# nginx-reverse-proxy-conf-generator

# A simple configuration generator for NGINX reverse proxies

# Generates .conf in the following format: 

    upstream [src_fqdn]
    {
        server [dest_fqdn]:[dest_port];
    }

    server
    {
        listen *:[src_port];
        server_name [src_fqdn];

        location /
        {
            proxy_pass https://[src_fqdn];
            proxy_set_header        Host            $host;
            proxy_set_header        X-Real-IP       $remote_addr;
            proxy_set_header        X-Forwarded-For $proxy_add_x_forwarded_for;
            proxy_connect_timeout 180;
            proxy_send_timeout 180;
            proxy_read_timeout 180;
        }
    }
