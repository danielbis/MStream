import json
import sys

dataDir = "/home/danielbis/Desktop/mstream/data/"
for f in ["Tweets", "Tweets-T", "News", "News-T"]:
    temp_dir = dataDir + f
    print("Converting %s from json to tab separated inputs, saving to %s " % (f, f.lower().replace('-', '_')))
    with open(temp_dir) as input:
        with open(dataDir + f.lower(), 'w') as out:
            for line in input:
                obj = json.loads(line)
                new_line = "%s\t%s\n" % (obj['Id'], obj['textCleaned'])
                out.write(new_line)

print("Done!")