import time, os, network
import uasyncio as asyncio

formpassword = "wpass"

rl1 = b"<html><body><h1>config.txt</h1>\n"
rl2 = b'<form action="/">\n'
rl3 = b"<table>\n"
rl5 = b'</table><input type="submit"></form>\n'
rl8 = b'</body></html>\n'

async def serveconfigfile(w, sget, msg):
    global formpassword
    print("serveconfigfile", sget, msg)
    rl4s = [ ]
    for l in open("config.txt"):
        k = l.split()
        if len(k) == 2:
            if k[0] == "password":
                formpassword = k[1]
            else:
                rl4s.append(b'  <tr><td>{0}:</td><td><input name="{0}" type="text" value="{1}"></td></tr>\n'.format(k[0], k[1]))
    rl4s.append(b'  <tr><th>password:</th><th><input name="password" type="text" value=""></th></tr>\n'.format(k[0], k[1]))
    rl6 = b'<p style="color:red">{}</p>\n'.format(msg or "")
    rl7 = b"<pre>{}</pre>\n".format(sget)
    cl = len(rl1)+len(rl2)+len(rl3)+sum(map(len, rl4s))+len(rl5)+len(rl6)+len(rl7)
    print("serveconfigfile", sget, msg, "length", cl)

    await w.awrite(b"HTTP/1.0 200 OK\r\n")
    await w.awrite(b"Connection: close\r\n")
    await w.awrite(b"Content-Type: text/html\r\n")
    await w.awrite(b"Content-Length: %d\r\n" % cl)
    await w.awrite(b"\r\n")
    await w.awrite(rl1)
    await w.awrite(rl2)
    await w.awrite(rl3)
    for rl4 in rl4s:
        await w.awrite(rl4)
    await w.awrite(rl5)
    await w.awrite(rl6)
    await w.awrite(rl7)
    print("configfile served")
    
def urldecode(v):
    return "".join((p.replace("+", " ") if i == 0 else \
                    chr(int(p[:2], 16))+p[2:].replace("+", " ")) \
                      for i, p in enumerate(v.split("%")))
def saveconfigdata(sget):
    password = None
    fout = open("Dconfig.txt", "w")
    for l in sget.split("&"):
        k, v = tuple(map(urldecode, l.split("=")))
        if k == "password":
            password = v
        m = k+(" "*max(1,16-len(k)))+v+"\n"
        fout.write(m)
        print(m)
    fout.close()
    if password == formpassword:
        os.rename("Dconfig.txt", "config.txt")
        return "saved"
    if password == "reboot":
        import machine
        machine.reset()
    return "wrong password"

async def readhttpheaderget(r):
    l = await r.readline()
    h = l.decode().split()
    print(h)
    if len(h) != 3 or h[0] != "GET":
        return None
    while l != b"\r\n":
        l = await r.readline()
        print(l)
    return h[1]
        

async def handleconnection(reader, writer):
    try:
        sget = await readhttpheaderget(reader)
        print("sget:", sget)
        if sget is not None:
            msg = ""
            if len(sget) >= 2 and sget[1] == "?":
                msg = saveconfigdata(sget[2:])
            await serveconfigfile(writer, sget, msg)   
        await writer.aclose()
        print("writer closed")
    except OSError as e:
        print("handleconnectionerror", [e])
        

