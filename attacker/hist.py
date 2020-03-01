#!/usr/bin/env python3

import sys
import binascii

def diff(s1, s2):
    idx = set([])
    for i in range(len(s1)):
        if s1[i] != s2[i]:
            idx.add(i)
    return frozenset(idx)

def dist(s1, s2):
    return len(diff(s1, s2))

if __name__ == '__main__':

    with open(sys.argv[1], 'r') as f:
        samples = map(str.strip, f.readlines())
        samples = [binascii.unhexlify(s) for s in samples]

    classes = {}
    correct = binascii.unhexlify(sys.argv[2])
    hist = {k: 0 for k in range(17)}

    for sample in samples:
        f = diff(correct, sample)
        hist[len(f)] += 1
        try:
            classes[f].append(sample)
        except KeyError:
            classes[f] = [sample]

    ## create a plot ##

    x = []
    y = []
    for k, v in hist.items():
        x.append(k)
        y.append(v)

    import matplotlib.pyplot as plt

    width = 0.75
    plt.figure(figsize=(8,5))
    plt.bar(x, y, width, color='grey')
    plt.xticks(range(17))
    plt.xlabel('Hamming Distance (Bytes)')
    plt.ylabel('Occurrences')

    for k, v in hist.items():
        plt.text(k, v + 60, str(v), color='black', fontweight='bold', va='center', ha='center')

    plt.savefig('hist.svg', dpi=300, format='svg', bbox_inches='tight')
