from typing import TextIO
from itertools import islice
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
        __bar: str = "=" * int(__pct * 10) + " " * (10 - int(__pct * 10)) 
        print(f"| Rendering: [{__bar}] - {round(__pct * 100, 3)}% - {self.cur} / {self.m_f} |", end="\r")


def _min_max(line: str) -> tuple[float, float]:
    _l: np.ndarray = np.fromstring(line.strip(), sep=",", dtype=np.float32)
    return (np.min(_l), np.max(_l))


def _parse_context(read_in: str) -> dict:
    _ctx_list = read_in.strip().split(",")
    context_dict = {"name":_ctx_list[0][:-4],
                    "height":float(_ctx_list[1]),
                    "width":float(_ctx_list[2]),
                    "dpi":int(_ctx_list[3]),
                    "channels":max(int(_ctx_list[4])+1, 2),
                    "numframes":int(_ctx_list[5]),
                    "framerate":int(_ctx_list[6]),
                    "scale":_ctx_list[7],
                    "framesize":int(_ctx_list[8])}
    return context_dict


def _animate_frames(frames: TextIO, rnge: np.ndarray, context: dict) -> None:
    # Plot and Counter Setup
    _counter = _FrameCounter(context["numframes"])
    _f, _axList = plt.subplots(_chns := context["channels"], 1, 
                               figsize=(context["width"]/context["dpi"], context["height"]/context["dpi"]), 
                               dpi=context["dpi"])
    _f.subplots_adjust(0, 0, 1, 1)

    # Universal channel size expansion
    if _chns < 2:
        _axList = np.array([_axList])

    for a in range(_chns):
        _axList[a].set_xscale(context["scale"])
        _axList[a].set_xlim(1, context["framesize"])
        _axList[a].set_ylim(context["ymin"], context["ymax"])
        _axList[a].axis("off")
    
    # Animation and frame update setup
    _lines = [_axList[a].plot(rnge, np.zeros_like(rnge))[0] for a in range(_chns)]
    def _init_frame() -> list:
        for c in range(_chns):
            _lines[c].set_data(rnge, np.zeros_like(rnge))
        return _lines

    def _update_frame(_) -> list:
        for c in range(_chns):
            _npfrm: np.ndarray = np.fromstring(frames.readline().strip(), sep=",", dtype=np.float32)
            _lines[c].set_ydata(_npfrm)
        _counter._prog_bar()
        return _lines

    # Start animation render and file saving
    _render_anim: an.FuncAnimation = \
    an.FuncAnimation(_f, _update_frame, init_func=_init_frame, blit=True, repeat=False, cache_frame_data=False,
                     frames=context["numframes"]-1, interval=1000.0/context["framerate"])
    _render_anim.save(filename=f"{context['name']}.mp4", writer="ffmpeg", fps=context["framerate"])


# MAIN
if __name__ == "__main__":
    # Check path
    __path: str = "_Frame_DUMP.rvdn"
    if not os.path.isfile(__path):
        raise NotADirectoryError("Frames not found. Cannot continue.")
    
    # TODO: Maybe convert the float digits to Hexadecimal and convert them back here.
    
    # Get min & max range from file
    _min: float; _max: float
    with open(__path, "r") as file:
        _stats: list = [_min_max(line) for line in islice(file, 2, None)]
        _min = min(_stats, key=lambda p: p[0])[0]
        _max = max(_stats, key=lambda p: p[1])[1]
        file.close()
    
    # Get context, ranges, and frames, then pass into updating function
    plt.tight_layout()
    with open(__path, "r") as file:
        _context: dict = _parse_context(file.readline().strip())
        _context.update({"ymin":_min, "ymax":_max})
        _range: np.ndarray = np.arange(_context["framesize"])+1
        _animate_frames(file, _range, _context)
        file.close()

    # Finish and close
    print("\r| Render complete. |" + " "*30)
    os.remove(__path)