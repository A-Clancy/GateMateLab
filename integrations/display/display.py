import threading
import queue
import time

from sense_hat import SenseHat


class SenseHatDisplay:
    def __init__(self, low_light: bool = True, scroll_speed: float = 0.06):
        self._sense = SenseHat()
        self._sense.low_light = low_light
        self._scroll_speed = scroll_speed
        self._q: "queue.Queue[tuple[str, str]]" = queue.Queue()
        self._last_state: str | None = None

        self._t = threading.Thread(target=self._worker, daemon=True)
        self._t.start()

    def update_presence(self, vehicle_present: bool, scroll_on_change: bool = True) -> None:
        state = "present" if vehicle_present else "clear"

        if scroll_on_change and state != self._last_state:
            self._last_state = state
            text = "Vehicle present" if vehicle_present else "Gate clear"
            self._enqueue(state, text)
        else:
            self._enqueue(state, "")

    def _set_matrix(self, state: str) -> None:
        if state == "present":
            self._sense.clear(255, 0, 0)  # red
        else:
            self._sense.clear(0, 255, 0)  # green

    def _scroll(self, text: str) -> None:
        self._sense.show_message(text, scroll_speed=self._scroll_speed)
        time.sleep(0.2)

    def _enqueue(self, state: str, text: str) -> None:
        # Drop backlog: only latest display intent matters.
        try:
            while True:
                self._q.get_nowait()
                self._q.task_done()
        except queue.Empty:
            pass
        self._q.put((state, text))

    def _worker(self) -> None:
        while True:
            state, text = self._q.get()
            try:
                self._set_matrix(state)
                if text:
                    self._scroll(text)
            except Exception:
                # Never crash the subscriber because of display issues.
                pass
            finally:
                self._q.task_done()