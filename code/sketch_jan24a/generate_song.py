# Short script to generate a song as an array of Note structs in C

cur_line = 1
with open('tmp') as fp:
    line = fp.readline()
    while line:
        print("const Note song{} PROGMEM {}".format(cur_line, line.strip()))
        line = fp.readline()
        cur_line += 1

for i in range(1,cur_line):
    print("song{},".format(i))
