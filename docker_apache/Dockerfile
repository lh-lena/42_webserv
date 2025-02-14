# Use an official Debian or Ubuntu base image
FROM debian:bullseye

# Install Apache and necessary tools
RUN apt-get update && \
apt-get install -y apache2 apache2-utils libapache2-mod-php php python3 && \
apt-get clean

# Enable CGI module
RUN a2enmod cgi


# Configure Apache to handle .py (Python) and .php (PHP) scripts
RUN echo ' \
<VirtualHost *:80> \
    DocumentRoot /var/www/html \
    \
    # CGI Configuration for Python \
    ScriptAlias /cgi-bin/ /usr/lib/cgi-bin/ \
    <Directory "/usr/lib/cgi-bin"> \
        AllowOverride None \
        Options +ExecCGI \
        AddHandler cgi-script .py\
        Require all granted \
    </Directory> \
    ErrorLog ${APACHE_LOG_DIR}/error.log \
    CustomLog ${APACHE_LOG_DIR}/access.log combined \
</VirtualHost>' > /etc/apache2/sites-available/000-default.conf

# Set up the CGI-BIN directory
RUN mkdir -p /usr/lib/cgi-bin && \
    chown -R www-data:www-data /usr/lib/cgi-bin && \
    chmod 755 /usr/lib/cgi-bin

# Copy a sample CGI script
COPY test.py /usr/lib/cgi-bin/test.py


# Give execute permission to the script
RUN chmod +x /usr/lib/cgi-bin/test.py

# Expose the default HTTP port
EXPOSE 80

# Set Apache to run in the foreground
CMD ["/usr/sbin/apache2ctl", "-D", "FOREGROUND"]

