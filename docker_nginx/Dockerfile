FROM nginx:latest

WORKDIR /etc/nginx

COPY nginx.conf /etc/nginx/nginx.conf

COPY ../demo/var/ /usr/share/nginx/html/

RUN ls /usr/share/nginx/html/

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]

# docker build -t custom-nginx .
# docker run -p 8080:80 custom-nginx