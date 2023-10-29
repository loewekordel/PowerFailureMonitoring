#!/usr/bin/python3
"""
Power Failure Monitoring by monitoring an alive signal on thingspeak.
In case of a power failure detection, a failure information is sent
over simplepush.
"""

import os
import time
import json
import logging
import secrets
import requests
import thingspeak
import simplepush

ALIVE_COUNTER_TIMEOUT = 180  # s
CYCLE_TIME = 60  # s


def main() -> int:
    """
    Main function

    :return: 0 on success else >1
    """
    power_state = True
    log_name = os.path.join("PowerMonitoring.log")
    logging.basicConfig(
        filename=log_name,
        filemode="w",
        level=logging.INFO,
        format="%(asctime)s: %(levelname)s: %(message)s",
    )

    channel = thingspeak.Channel(
        id=secrets.THINGSPEAK_CHANNEL_ID, api_key=secrets.THINGSPEAK_API_KEY
    )

    starttime = time.time()
    try:
        while True:
            try:
                last_data_age = channel.get_last_data_age(1)
                last_data_age = json.loads(last_data_age)
                t = int(last_data_age["last_data_age"])
                logging.info(f"{t=}")
                if t > ALIVE_COUNTER_TIMEOUT and power_state:
                    power_state = False
                    last_entry = channel.get_field_last(
                        1, options={"timezone": "Europe/Vienna"}
                    )
                    last_entry = json.loads(last_entry)
                    logging.info(f"Power failure: {last_entry['created_at']}")
                    for key in secrets.SIMPLEPUSH_KEYS:
                        simplepush.send(
                            key, "Stromausfall", last_entry["created_at"], None
                        )
                elif t <= ALIVE_COUNTER_TIMEOUT and not power_state:
                    power_state = True
                    last_entry = channel.get_field_last(
                        1, options={"timezone": "Europe/Vienna"}
                    )
                    last_entry = json.loads(last_entry)
                    logging.info(f"Power restored: {last_entry['created_at']}")
                    for key in secrets.SIMPLEPUSH_KEYS:
                        simplepush.send(
                            secrets.SIMPLEPUSH_KEYS,
                            "Stromversorgung OK",
                            last_entry["created_at"],
                            None,
                        )
            except requests.exceptions.HTTPError as e:
                print(e)
                logging.error(e)

            # delay; avoid drift
            time.sleep(
                float(CYCLE_TIME) - ((time.time() - starttime) % float(CYCLE_TIME))
            )

    except KeyboardInterrupt:
        logging.error("End")


if __name__ == "__main__":
    raise SystemExit(main())
