FROM php:8.3-apache

# PostgreSQL (Neon). SQLite est déjà inclus dans l'image.
RUN apt-get update \
 && apt-get install -y --no-install-recommends libpq-dev \
 && docker-php-ext-install pdo_pgsql \
 && rm -rf /var/lib/apt/lists/*

# Durcissement : versions PHP/Apache masquées, pas de listing de dossiers
RUN echo "expose_php = Off" > /usr/local/etc/php/conf.d/securite.ini \
 && printf "ServerTokens Prod\nServerSignature Off\n<Directory /var/www/html>\n  Options -Indexes\n</Directory>\n" \
    > /etc/apache2/conf-enabled/zz-securite.conf

# Port fourni par Render
ENV PORT=10000
RUN sed -i 's/Listen 80/Listen ${PORT}/' /etc/apache2/ports.conf \
 && sed -i 's/<VirtualHost \*:80>/<VirtualHost *:${PORT}>/' /etc/apache2/sites-available/000-default.conf

# Base SQLite de secours, HORS du dossier public (impossible à télécharger)
RUN mkdir -p /var/www/data && chown www-data:www-data /var/www/data

# Le code reste propriété de root : Apache peut le lire, pas le modifier
COPY . /var/www/html/

EXPOSE 10000
