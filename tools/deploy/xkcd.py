#!/usr/bin/python

import json
import urllib

# Get latest xkcd data
url = "http://xkcd.com/info.0.json"
response = urllib.urlopen(url)
data = json.loads(response.read())

# Print the title, with proper escaping
print json.dumps(data["safe_title"]).strip('"')
