with open("out.txt", "r") as f:
    idx = 1
    arr = f.read().split('\n')
    o = open('1.out', 'w')
    for l in arr:
        print(l, file=o)
        if l == 'bye':
            o.close()
            idx += 1
            if idx <= 5:
                o = open('%d.out' % idx, 'w')
