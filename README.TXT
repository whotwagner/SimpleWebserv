THIS IS JUST A SIMPLE HTTP-SERVER
IT USES blocking accept() and fork()
Here we handle the HTTP-Requests.

Simple(just one line): GET /index.html

Multiline(ends with blank line): 
GET /index.html HTTP/1.0
Host: tech.feedyourhead.at
User-Agent: Mozilla Fireox Version/2.14
Accept-Encoding: gzip, compress
Accept-Language: en
<BLANK LINE>	

