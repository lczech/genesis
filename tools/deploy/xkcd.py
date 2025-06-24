#!/usr/bin/python3

import json
import urllib
import urllib.request

# Get latest xkcd data
xkcd_url = "http://xkcd.com/info.0.json"
with urllib.request.urlopen(xkcd_url) as url:
    response = url.read()
    data = json.loads(response)

# Print the title, with proper escaping
print(json.dumps(data["safe_title"]).strip('"'))
