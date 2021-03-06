// embedded_auth.h
// Weborf copyright 2010 (GPL3+) Salvo "LtWorf" Tomaselli <tiposchi@tiscali.it>

// This file contains user defined embedded autentication
// to avoid the usage of an external daemon for authentication.
// The use of this, will make things faster and parallel

extern weborf_configuration_t weborf_conf;                                      

// This function is just example code, can be changed or deleted
static int emb_check_password(char *username, char *password) {
    if (weborf_conf.user == NULL || weborf_conf.pass == NULL) return 0;
    else {
        if (strcmp(username,weborf_conf.user)==0 && strcmp(password,weborf_conf.pass)==0)
            return 0;
        return -1;
    }
}

// page:       URI of requested page
// ip_addr:    IP address of the client (it can be an IPv6, depending how weborf is compiled)
// method:     HTTP method of the request
// username:   Username, if provided, null otherwise
// password:   Password, if provided, null otherwise
// http_param: Headers of the request
// RETURN VALUE:  Return 0 to allow the request, -1 to deny it.
// NOTES: The actual content of the function must be regarded as an example,
// modify it according to your own needs.
// Only use reentrant calls in this function. Weborf is multithreaded.
static int c_auth(char *page, char *ip_addr, char *method, char *username, char *password, char *http_param) {
    return emb_check_password(username,password);
    char *allowed_prefix="::ffff:10.";
    char *foto = "/foto/";

    // Allow anything from 10.*
    if (strncmp(allowed_prefix,ip_addr,strlen(allowed_prefix))==0) return 0;

    // Allow PROPFIND and OPTIONS with authentication (to allow read only webdav from anywhere)
    if ((strncmp(method,"PROPFIND",strlen("PROPFIND"))==0) || (strncmp(method,"OPTIONS",strlen("OPTIONS"))==0))
        return emb_check_password(username,password);

    // Deny all except GET and POST
    else if (!(strncmp(method,"GET",strlen("GET"))==0 || strncmp(method,"POST",strlen("POST"))==0)) {
        return -1;
    }

    // request authentication for photos
    if (strncmp(foto,page,strlen(foto))==0)
        return emb_check_password(username,password);

    return 0;
}
