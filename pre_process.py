import json
import sys

dataDir = sys.argv[1]
print("Converting %s from json to tab separated inputs, saving to %s " % (dataDir, dataDir+"_clean"))
with open(dataDir) as input:
    with open(dataDir + "_clean", 'w') as out:
        for line in input:
            obj = json.loads(line)
            new_line = "%s\t%s\n" % (obj['Id'], obj['textCleaned'])
            out.write(new_line)

print("Done!")