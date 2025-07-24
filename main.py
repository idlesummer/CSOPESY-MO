from rich.console import Console
from rich.text import Text

# ASCII banner to colorize
ascii_art = """
  ██╗   ██████╗ ███████╗  ██████╗  ██████╗  ███████╗ ███████╗ ██╗   ██╗  ██╗  
 ██╔╝  ██╔════╝ ██╔════╝ ██╔═══██╗ ██╔══██╗ ██╔════╝ ██╔════╝ ╚██╗ ██╔╝  ╚██╗ 
██╔╝   ██║      ███████╗ ██║   ██║ ██████╔╝ █████╗   ███████╗  ╚████╔╝    ╚██╗
╚██╗   ██║      ╚════██║ ██║   ██║ ██╔═══╝  ██╔══╝   ╚════██║   ╚██╔╝     ██╔╝
 ╚██╗  ╚██████╗ ███████║ ╚██████╔╝ ██║      ███████╗ ███████║    ██║     ██╔╝ 
  ╚═╝   ╚═════╝ ╚══════╝  ╚═════╝  ╚═╝      ╚══════╝ ╚══════╝    ╚═╝     ╚═╝
""".strip("\n")

ascii_lines = ascii_art.splitlines()
max_len = max(len(line) for line in ascii_lines)

# Resize image to match widest line
gradient_rgb = [
  (201, 254, 221), (209, 249, 207), (218, 243, 197), (227, 236, 184),
  (234, 232, 170), (239, 224, 156), (248, 218, 142), (250, 206, 138),
  (249, 192, 138), (249, 179, 137), (249, 165, 134), (249, 150, 130),
  (249, 135, 126), (249, 121, 121)
]  # List of (R,G,B)


# === Convert RGB to ANSI 256 ===
def rgb_to_ansi256(r, g, b):
    r, g, b = [int(x * 5 / 255) for x in (r, g, b)]
    return 16 + 36 * r + 6 * g + b

# === Rich console rendering ===
console = Console()
console.print("[bold]MegaTron Gradient Banner[/bold]:")

def make_gradient(start_rgb, end_rgb, steps):
    return [
        tuple(int(start + (end - start) * t / (steps - 1))
              for start, end in zip(start_rgb, end_rgb))
        for t in range(steps)
    ]

# MegaTron colors: mint → peach-pink
start_rgb = (201, 254, 221)
end_rgb   = (249, 121, 121)
gradient_rgb = make_gradient(start_rgb, end_rgb, 100)

def get_color_from_gradient(i, width):
    index = int(i / max(width - 1, 1) * (len(gradient_rgb) - 1))
    return rgb_to_ansi256(*gradient_rgb[index])


# === Render with gradient ===
for line in ascii_lines:
    styled = Text()
    width = len(line)
    for i, ch in enumerate(line):
        color = get_color_from_gradient(i, width)
        styled.append(ch, style=f"color({color})")
    console.print(styled)
