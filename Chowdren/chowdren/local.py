from chowdren.codewriter import CodeWriter

class Locale(object):
    def get_string(self, source):
        return None

    def write(self, lang):
        pass

class DictLocale(Locale):
    def __init__(self, values):
        self.values = values

    def get_string(self, source):
        return self.values.get(source, None)

def write_locals(locales, writer, header, converter):
    header.putlnc('void set_local(const std::string & name);')
    writer.putmeth('void set_local', 'const std::string & name')

    if not locales:
        writer.end_brace()
        return

    writer.putln('static std::string current;')
    writer.putln('if (name == current) return;')
    writer.putln('current = name;')

    writer.putln('if (name.empty()) {')
    writer.indent()
    writer.putlnc('return;')
    writer.end_brace()

    for k, locale in locales.iteritems():
        writer.putlnc('if (name == %r) {', k, cpp=False)
        writer.indent()

        write_text_locales(locale, writer, header, converter)
        locale.write(writer, header, converter)
        writer.putln('return;')

        writer.end_brace()

    writer.end_brace()

def write_text_locales(locale, writer, header, converter):
    for value, name in converter.strings.iteritems():
        new_string = None
        if locale is not None:
            new_string = locale.get_string(value)
        if new_string is None:
            new_string = value
        writer.putlnc('%s.assign(%r, %s);', name, new_string,
                      len(new_string), cpp=False)
