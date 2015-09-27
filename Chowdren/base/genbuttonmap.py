maps = {
    'CHOWDREN_SNES_CONTROLLER': (
        ('CHOWDREN_BUTTON_X', 'CHOWDREN_BUTTON_A'),
        ('CHOWDREN_BUTTON_Y', 'CHOWDREN_BUTTON_X'),
        ('CHOWDREN_BUTTON_GUIDE', 'CHOWDREN_BUTTON_RIGHTSHOULDER'),
        ('CHOWDREN_BUTTON_BACK', 'CHOWDREN_BUTTON_LEFTSHOULDER'),
        ('CHOWDREN_BUTTON_LEFTSHOULDER', 'CHOWDREN_BUTTON_START'),
        ('CHOWDREN_BUTTON_B', 'CHOWDREN_BUTTON_B')
    ),
    'CHOWDREN_JOYSTICK2_CONTROLLER': (
        (5, 'CHOWDREN_BUTTON_LEFTSHOULDER'),
        (6, 'CHOWDREN_BUTTON_RIGHTSHOULDER'),
        (7, 'CHOWDREN_BUTTON_BACK'),
        (8, 'CHOWDREN_BUTTON_START'),
        (9, 'CHOWDREN_BUTTON_LEFTSTICK'),
        (10, 'CHOWDREN_BUTTON_RIGHTSTICK'),
        (11, 'CHOWDREN_BUTTON_GUIDE')
    )
}

def main():
    import sys
    sys.path.append('..')
    sys.path.append('../..')
    from chowdren.codewriter import CodeWriter
    writer = CodeWriter('buttonmap.cpp')
    writer.putmeth('int remap_button', 'int n')
    for k, v in maps.iteritems():
        writer.putraw('#ifdef %s' % k)
        writer.putlnc('switch (n) {')
        writer.indent()
        for f, t in v:
            writer.putlnc('case %s: return %s;', f, t)
        writer.end_brace()
        writer.putraw('#endif')
    writer.putlnc('return n;')
    writer.end_brace()

    writer.putmeth('int unremap_button', 'int n')
    for k, v in maps.iteritems():
        writer.putraw('#ifdef %s' % k)
        writer.putlnc('switch (n) {')
        writer.indent()
        for f, t in v:
            writer.putlnc('case %s: return %s;', t, f)
        writer.end_brace()
        writer.putraw('#endif')
    writer.putlnc('return n;')
    writer.end_brace()
    writer.close()

if __name__ == '__main__':
    main()
