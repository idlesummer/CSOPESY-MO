import shutil
from RichText import *


class RichAlign:
    def __init__(self, width=None):
        if width is None:
            width = shutil.get_terminal_size(fallback=(80, 20)).columns
        self.width = width

    def align(self, text, mode="left"):
        if isinstance(text, str):
            text = RichText(text)
        elif not isinstance(text, RichText):
            raise TypeError("Expected RichText or str")

        lines = text.splitlines()
        rendered_lines = [str(RichText(line)) for line in lines]  # rewrap each line
        visual_lengths = [RichText(line).size() for line in lines]

        result = []
        for raw, rendered, vislen in zip(lines, rendered_lines, visual_lengths):
            if mode == "left":
                padding = self.width - vislen
                result.append(rendered + " " * max(padding, 0))
            elif mode == "right":
                padding = self.width - vislen
                result.append(" " * max(padding, 0) + rendered)
            elif mode == "center":
                total_padding = self.width - vislen
                left = total_padding // 2
                right = total_padding - left
                result.append(" " * max(left, 0) + rendered + " " * max(right, 0))
            else:
                raise ValueError(f"Unknown mode: {mode}")
        return "\n".join(result)



if __name__ == "__main__":
    print("\n=== LEFT ALIGN ===")
    print("|" + RichAlign(30).align("hello world", "left") + "|")
    print("|" + RichAlign(30).align(RichText("[b]bold[/b] and [fg=red]red[/fg]"), "left") + "|")

    print("\n=== RIGHT ALIGN ===")
    print("|" + RichAlign(30).align("hello world", "right") + "|")
    print("|" + RichAlign(30).align(RichText("[i]italic[/i] and [u]underlined[/u]"), "right") + "|")

    print("\n=== CENTER ALIGN ===")
    print("|" + RichAlign(30).align("hello world", "center") + "|")
    print("|" + RichAlign(30).align(RichText("[fg=green]centered green[/fg]"), "center") + "|")

    print("\n=== MULTILINE EXAMPLE ===")
    multiline = "[b]Line 1[/b]\n[fg=red]Line 2 red[/fg]\n[b fg=green]Line 3 green bold[/b]"
    print(RichAlign(35).align(RichText(multiline), "center"))

    print("\n=== DEFAULT TERMINAL WIDTH EXAMPLE ===")
    long_line = "[b fg=cyan]This line should be centered using the full width of the terminal[/b]"
    print(RichAlign().align(RichText(long_line), "center"))
