constants = {}
with open('../Arduino/ProtoWi/CmdResponseDefinitions.h') as infile:
    for name, value in re.findall(r'#define\s+(\w+)\s+(.*)', infile):
        try:
            constants[name] = ast.literal_eval(value)
        except Exception as e:
            pass # maybe log something
with open('_foo_h.py', w) as outfile:
    outfile.write(repr(constants))