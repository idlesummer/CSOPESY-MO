from rich.console import Console
from rich.text import Text

# Step 1: Banner lines
ascii_art = """
  ██╗  ██████╗ ███████╗  ██████╗  ██████╗  ███████╗ ███████╗ ██╗   ██╗ ██╗  
 ██╔╝ ██╔════╝ ██╔════╝ ██╔═══██╗ ██╔══██╗ ██╔════╝ ██╔════╝ ╚██╗ ██╔╝ ╚██╗ 
██╔╝  ██║      ███████╗ ██║   ██║ ██████╔╝ █████╗   ███████╗  ╚████╔╝   ╚██╗
╚██╗  ██║      ╚════██║ ██║   ██║ ██╔═══╝  ██╔══╝   ╚════██║   ╚██╔╝    ██╔╝
 ╚██╗ ╚██████╗ ███████║ ╚██████╔╝ ██║      ███████╗ ███████║    ██║    ██╔╝ 
  ╚═╝  ╚═════╝ ╚══════╝  ╚═════╝  ╚═╝      ╚══════╝ ╚══════╝    ╚═╝    ╚═╝
""".strip("\n")

ascii_lines = ascii_art.splitlines()

# Step 2: Gradient setup (Top: mint → Bottom: coral)
mint_rgb = (201, 254, 221)
coral_rgb = (249, 121, 121)


def make_gradient(start_rgb, end_rgb, steps):
    gradient = []
    for t in range(steps):
        ratio = t / (steps - 1)
        r = int(start_rgb[0] + (end_rgb[0] - start_rgb[0]) * ratio)
        g = int(start_rgb[1] + (end_rgb[1] - start_rgb[1]) * ratio)
        b = int(start_rgb[2] + (end_rgb[2] - start_rgb[2]) * ratio)
        gradient.append((r, g, b))

    return gradient


def rgb_to_ansi256(r, g, b):
    r = int(r * 5 / 255)
    g = int(g * 5 / 255)
    b = int(b * 5 / 255)
    return 16 + 36 * r + 6 * g + b


gradient_rgb = make_gradient(coral_rgb, mint_rgb, len(ascii_lines))

# Step 3: Render with vertical gradient
console = Console()
console.print("[bold]MegaTron Gradient Banner (Vertical Gradient)[/bold]:")

for i, line in enumerate(ascii_lines):
    rgb = gradient_rgb[i]
    color = rgb_to_ansi256(*rgb)
    styled = Text(line, style=f"color({color})")
    console.print(styled)
