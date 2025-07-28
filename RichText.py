import re


class StyleStack:
    def __init__(self):
        self.stack = []

    def push(self, STYLES):
        for key, value in STYLES.items():
            self.stack.append({key: value})

    def pop(self, tag, all=False):
        if all:
            self.stack = [layer for layer in self.stack if tag not in layer]
            return
        
        for i in reversed(range(len(self.stack))):
            if tag in self.stack[i]:
                self.stack.pop(i)
                break

    def reset(self):
        self.stack.clear()

    def get_styles(self):
        styles = {}
        for layer in self.stack:
            styles.update(layer)
        return styles


class RichText:
    
    TAG_PATTERN = re.compile(r'\[(/?)([^\[\]]+?)\]')  # Matches [tag] and [/tag]
    PLACE_LB = "\x01"  # placeholder for \[
    PLACE_RB = "\x02"  # placeholder for \]
    ANSI_RESET = "\033[0m"
    NAMED_COLORS = {
        "black":   "#000000",
        "red":     "#ff0000",
        "green":   "#00ff00",
        "yellow":  "#ffff00",
        "blue":    "#0000ff",
        "magenta": "#ff00ff",
        "cyan":    "#00ffff",
        "white":   "#ffffff",
        "gray":    "#808080",
        "lightgray": "#cccccc",
        "darkred": "#880000",
        "darkgreen": "#008800",
        "darkblue": "#000088",
    }
    ALIASES = {
        "b": "bold",
        "i": "italic",
        "u": "underline",
        "s": "strike",
        "d": "dim"
    }
    STYLES = {
        "bold": "1",
        "dim": "2",
        "italic": "3",
        "underline": "4",
        "strike": "9",
    }

    def __init__(self, text=""):
        self.text = text
        self.stack = StyleStack()
        self.visual_size = None
        self.out = self.render()
        
    def parse(self, content):
        result = {}
        tokens = content.split(' ')

        for token in tokens:
            if '=' in token:
                key, val = token.split('=', 1)
                key = self.ALIASES.get(key) or key
                result[key] = val
            else:
                key = self.ALIASES.get(token) or token
                result[key] = True

        return result
    
    def hex_to_rgb(self, value):
        if value in self.NAMED_COLORS:
            value = self.NAMED_COLORS[value]
        value = value.lstrip("#")
        return tuple(int(value[i:i+2], 16) for i in (0, 2, 4))
        
    def to_ansi(self, styles):
        codes = []

        for key, value in styles.items():
            if key in self.STYLES:
                codes.append(self.STYLES[key])
            elif key == "fg":
                r, g, b = self.hex_to_rgb(value)
                codes.append(f"38;2;{r};{g};{b}")
            elif key == "bg":
                r, g, b = self.hex_to_rgb(value)
                codes.append(f"48;2;{r};{g};{b}")

        return f"\033[{';'.join(codes)}m" if codes else ''
    
    def render(self):
        output = ""
        index = 0
        text = self.text.replace("\\[", self.PLACE_LB).replace("\\]", self.PLACE_RB)

        while index < len(text):
            match = self.TAG_PATTERN.search(text, index)
            if not match:
                output += text[index:]
                break

            start, end = match.span()
            output += text[index:start]
            is_closing = match.group(1) == '/'
            tag_content = match.group(2).strip()

            if is_closing:
                if tag_content == "":
                    self.stack.reset()
                    output += self.ANSI_RESET
                else:
                    resolved = self.ALIASES.get(tag_content, tag_content)
                    self.stack.pop(resolved)
                    output += self.ANSI_RESET + self.to_ansi(self.stack.get_styles())
            else:
                styles = self.parse(tag_content)
                self.stack.push(styles)
                output += self.to_ansi(styles)

            index = end

        output = output.replace(self.PLACE_LB, "[").replace(self.PLACE_RB, "]")
        output += self.ANSI_RESET
        self.visual_size = len(self.strip_ansi(output))
        return output
    
    def __str__(self):
        return self.out
    
    def __len__(self):
        return self.visual_size
    
    def strip_ansi(self, text):
        ansi_escape = re.compile(r'\033\[[0-9;]*m')
        return ansi_escape.sub('', text)


if __name__ == "__main__":
    print(RichText("Normal [b]Bold[/b] Normal"))
    print(RichText("[i]Italic[/i] then [u]Underline[/u]"))
    print(RichText("This is [fg=red]red[/fg] and [fg=#00ff00]green hex[/fg]"))
    print(RichText("Now with background: [bg=yellow]yellow bg[/bg]"))
    print(RichText("[b fg=red]Red Bold [fg=blue]Blue[/fg] Red Bold Again[/b]"))
    print(RichText("Literal brackets: \\[b\\]not bold\\[/b\\]"))
    print(RichText("[bold fg=green]Green and Bold (unclosed)"))
    print(RichText("[fg=red u]Red Underline[/fg]"))
    print(RichText("[b fg=blue]Blue Bold[/b]"))
    print(RichText("[fg=red][fg=blue]Colored [/fg][/fg]No color"))
    print(len(RichText("[b]Testing[/]")))

