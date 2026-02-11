import re


def preprocess(md: str) -> str:
    md = re.sub(
            r"\s--cols--(.+?)--cols--\s", 
            r"\n<div class='cols'><div>\n\1\n</div></div>\n", 
            md, 
            flags=re.MULTILINE|re.DOTALL
        )

    md = re.sub(
            r"\s--c--\s", 
            r"\n</div><div>\n", 
            md
        )
   
    return md