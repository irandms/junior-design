cur_line = 1
with open('tmp') as fp:
    line = fp.readline()
    while line:
        print("const Note galaga{} PROGMEM {}".format(cur_line, line.strip()))
        line = fp.readline()
        cur_line += 1

for i in range(1,cur_line):
    print("galaga{},".format(i))
