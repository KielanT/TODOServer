# TODO List Project: Server
- Desktop: https://github.com/KielanT/TODODesktop
- Web app: https://github.com/KielanT/TODOWeb

## Description
TODO Server is a C++ server that allows users to create and manage to-do lists and tasks that are stored on a C++ server with a MySQL database. The front ends which are linked above communicates with the server via HTTP requests. I run this server on a raspberry pi

The project integrates Google OAuth 2.0 for user authentication this is done on the frontends. The user's email, first name, and Google ID are stored in the database which ensures that users can only access their own tasks and lists.

## Features
- Create, update and delete lists and tasks
- HTTP request
- Login Via Google OAuth 2.0

## Libraries 
- Crow
