import matplotlib.pyplot as plt
import matplotlib.animation as an
import numpy as np
import os

class _FrameCounter():
    def __init__(self, max_frames: int) -> None:
        self.m_f: int = max_frames
        self.cur: int = 0
    
    def _prog_bar(self) -> None:
        self.cur += 1
        __pct = self.cur / self.m_f
        __bar: str = "█" * int(__pct) + " " * (10 - int(__pct)) 
        print(f"| Rendering: [{__bar}] - {round(__pct * 100, 3)}% - {self.cur} / {self.m_f} |", end="\r")


def _parse_frames(frames: list[str] | str, channels: int = 1) -> list[np.ndarray] | np.ndarray:
    if type(frames) is str:
        return np.array(frames.split(",")).astype("float")
    return [np.array([c.split(",") for c in frames[i:i+channels]]).astype("float") 
            for i in range(len(frames), step=channels)]


def _parse_context(read_in: str) -> dict:

    # TODO: REQUIRED COMPONENTS:
    # min & max values (for both channels if possible)
    # mono status (num channels)
    # window resolution (dpi)
    # numframes
    # scale
    # framerate
    # name (file name)
    # ~ON SECOND LINE~ range array
    # Data Lines MUST BE  --->  L\n  R\n. Reshape to (2, ...) if stereo. 

    pass


def _animate_frames(frames: list[np.ndarray], rnge: np.ndarray, context: dict) -> None:
    # Plot and Counter Setup
    _counter = _FrameCounter(context["numframes"])
    _f, _axList = plt.subplots(_chns := context["channels"], 1, figsize=(16, 9))
    _f.subplots_adjust(0, 0, 1, 1)

    # Universal channel size expansion
    if _chns < 2:
        _axList = np.array([_axList])
    for a in range(_chns):
        _axList[a].set_xscale(context["scale"])
        _axList[a].axis("off")
    
    # Animation and frame update setup
    _lines = [_axList[a].plot(rnge, np.zeros_like(frames[0][a]))[0] for a in range(_chns)]
    def _update_frame(frm) -> None:
        for c in range(_chns):
            _lines[c].set_ydata(frm[c])
        _counter._prog_bar()

    # Start animation render and file saving
    _render_anim: an.FuncAnimation = \
    an.FuncAnimation(_f, _update_frame, frames=frames, interval=1000.0/context["framerate"])
    _render_anim.save(filename=f"{context['name']}.mp4", writer="ffmpeg")


# MAIN
if __name__ == "__main__":
    # Check path
    __path: str = "_Frame_DUMP.rvdn"
    if not os.path.exists(__path):
        raise NotADirectoryError("Frames not found. Cannot continue.")
    
    # Get file, dump data into string list, and clear file space
    _all_lines: list[str] = None
    with open("_Frame_DUMP.rvdn", "r") as file:
        _all_lines: list[str] = file.readlines()
        file.close()

    # Get context, ranges, and frames, then pass into updating function
    _context: dict = _parse_context(_all_lines[0])
    _range: np.ndarray = _parse_frames(_all_lines[1])
    _frames: list[np.ndarray] = _parse_frames(_all_lines[2:], _context["channels"])
    plt.tight_layout()
    _animate_frames(_frames, _range, _context)

    # Finish and close
    print("| Render complete. |" + " "*30, end="\r")
    os.remove(__path)