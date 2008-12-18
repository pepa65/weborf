#!/usr/bin/python
'''
Weborf
Copyright (C) 2008  Salvo "LtWorf" Tomaselli

Weborf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@author Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>
'''

import sys
import py_compile
import os
import socket
import base64
import csv
import time

def hideErrors():
    '''Closes stderr so errors aren't shown anymore'''
    os.close(2)

def redirect(location):
    '''Sends to the client the request to redirect to another page.
    Unless php, here this can be used even after output'''
    os.write(4,"Location: "+location+"\r\n") #Writes location header
    sys.exit(33) #Tells weborf that a redirect is requested

def post_escape(val):
    '''Post fields use certains escapes. This function returns the original string'''
    val=val.replace("+"," ") #Replaces all + with a space
    
    i=val.find("%") #% is the char for an exadecimal escape
    while i!=-1: #If there is a % in the non parsed part of the string
        s=val[i+1] + val[i+2] #Extract the exadecimal code
        if s!="37":
            #Replaces all the escapes in the string
            val=val.replace("%"+s,chr(int(s,16)))
        else:
            '''Replaces only once because this char is a % so there would be %
            that aren't escapes in the non parsed part of the string'''
            val=val.replace("%"+s,chr(int(s,16)),1)
            
        i=val.find("%",i+1)
    
    return val
def getVal(dic,key):
    '''Returns the value dic[key] or None if the key doesn't exist'''
    try:
        return dic[key]
    except:
        return None
def setcookie(name,value,expires=None):
    '''Sets a cookie, by default it will be a session cookie.
    Expires is the time in seconds to wait to make the cookie expire'''
    os.write(4,"Set-Cookie: "+str(name)+ "=" + str(value))
    if expires!=None:
        os.write(4,"; Max-Age="+str(expires))
    os.write(4,"\r\n")


def session_start():   
    '''Inits the session vars'''
    s_id=getVal(_COOKIE,'PHPSESSID')#Gets the session ID
    
    if s_id==None: #No session, creating a new one
        import random
        import md5
        
        #Creating session's id with random numbers and multiple hashes
        r=random.Random()
        
        a=md5.md5(sys.argv[6]).hexdigest()+md5.md5(str(r.random())).hexdigest()
        for i in range(10):
            a=md5.md5(a).hexdigest()+md5.md5(str(r.random())).hexdigest()
        
        s_id= "weborf-"+ str(os.getpid())+ "-" + a
        setcookie('PHPSESSID',s_id)
        _COOKIE['PHPSESSID']=s_id
    else:#Session exists, loading data
        try:
            #If session expired after inactivity
            if (os.stat(TMPDIR+"/"+_COOKIE['PHPSESSID'])[7] + SESSIONEXPIRE) < time.time():
                #Deletes old session file, just to try to avoid to fill the disk
                os.unlink(TMPDIR+"/"+_COOKIE['PHPSESSID'])
                #Creating an empty session
                _COOKIE['PHPSESSID']=None
                session_start()
                return
            
            fp=file(TMPDIR+"/"+_COOKIE['PHPSESSID'])
            reader=csv.reader(fp) #Creating a csv reader
            for i in reader.__iter__(): #Iterating rows
                _SESSION[i[0]]=i[1]
        except:        
            #Start sessions with a new session id
            _COOKIE['PHPSESSID']=None
            session_start()

def savesession():
    '''Saves the session to the file'''
    if _COOKIE['PHPSESSID']==None:
        return #No session to save
    #Opens the file with the session
    fp=file(TMPDIR+"/"+_COOKIE['PHPSESSID'],"w")

    writer=csv.writer(fp)

    #Converting dictionary into 2 level array for csv module
    a=[]
    for i in _SESSION:
        a.append((i,_SESSION[i]))
    writer.writerows(a)
    fp.close()


#Loading configuration from file or setting default
try:
    execfile("/etc/weborf/pywrapper.conf")
except:
    TMPDIR="/tmp"
    SESSIONEXPIRE=600
    CONTENT="text/html"

#Sets SERVER and HEADER variables
_SERVER={}
_HEADER={}
fields = sys.argv[3].split("\r\n")
protocol=fields.pop(0)
fields.remove("")
for i in fields:
    v=i.split(": ",1)
    _HEADER[v[0]]=v[1]

_SERVER['SERVER_SOFTWARE']= os.getenv("WEBORF")
_SERVER['SERVER_SIGNATURE']=os.getenv("WEBORF")
_SERVER['SERVER_PORT']=os.getenv("WEBORF_PORT")
_SERVER["REQUEST_METHOD"]=sys.argv[5]
_SERVER["HTTP_REFERER"]=getVal(_HEADER,"Referer")
_SERVER["HTTP_CONNECTION"]=getVal(_HEADER,"Connection")
_SERVER['HTTP_ACCEPT_LANGUAGE']=getVal(_HEADER,"Accept-Language")
_SERVER['HTTP_ACCEPT_ENCODING' ]=getVal(_HEADER,"Accept-Encoding")
_SERVER['HTTP_ACCEPT_CHARSET' ]=getVal(_HEADER,"Accept-Charset")
_SERVER['HTTP_USER_AGENT']=getVal(_HEADER,'User-Agent')
_SERVER['SERVER_PROTOCOL']=protocol
_SERVER['SCRIPT_FILENAME']=sys.argv[1]
_SERVER['SCRIPT_NAME']=sys.argv[1]
_SERVER['HTTP_HOST']=getVal(_HEADER,'Host')
if _SERVER['HTTP_HOST']!=None:
    _SERVER['SERVER_NAME']=_SERVER['HTTP_HOST']
else:
    _SERVER['SERVER_NAME']=socket.gethostname()

_SERVER['REMOTE_ADDR']=sys.argv[6]
_SERVER['HTTPS']=None #Will have to do something better when ssl will be implemented!
_SERVER['REMOTE_HOST']=None
_SERVER['REMOTE_PORT']=None

#Deconding auth field
v=getVal(_HEADER,'Authorization')
if v!=None:
    q=v.split(" ")
    _SERVER['AUTH_TYPE']=q[0]
    auth=base64.b64decode(q[1]).split(":",1)
    _SERVER['PHP_AUTH_USER']=auth[0]
    _SERVER['PHP_AUTH_PW']=auth[1]
else:
    _SERVER['AUTH_TYPE']=None
    _SERVER['PHP_AUTH_USER']=None
    _SERVER['PHP_AUTH_PW']=None
    
#Sets POST variables
_POST={}
if len(sys.argv[4])!=0:
    for i in sys.argv[4].split("&"):
        v=i.split("=")
        _POST[post_escape(v[0])]=post_escape(v[1])

#Sets GET variables
_GET={}
if len(sys.argv[2])!=0:
    for i in sys.argv[2].split("&"):
        v=i.split("=")
        _GET[v[0]]=v[1]


#Sets SESSION variables
_SESSION={}

#Sets COOKIE variables
_COOKIE={}
if getVal (_HEADER,'Cookie')!=None:
    for i in _HEADER['Cookie'].split(";"):
        q=i.find("=")
        if q!=-1:
            _COOKIE[i[0:q].strip()]=i[q+1:].strip()
        else:
            _COOKIE[i.strip()]=None
            
#Changing dir to script's one
for i in range(len(sys.argv[1])-1,-1,-1):
    if sys.argv[1][i]==os.sep:
        os.chdir(sys.argv[1][0:i])
        break

#Executes file
execfile(sys.argv[1])

#Saves session, if there is one
savesession()

if CONTENT!=None:
    os.write(4,"Content-Type: "+CONTENT+"\r\n") #Writes content type, by default html

sys.exit(0)