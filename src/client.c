// (C) Thomas Baumeister, 2024
// For further information read the comment at the end of the file.

#include <stdbool.h>

#include "client.h"
#include "client_interface.h"

bool
is_empty (const char *str)
{
  return str[0] == '\0';
}

char *username = "";
void
msg_wuser (char **msg, const char *input)
{
  if (is_empty (username))
    {
      perror ("Username empty?");
      exit (1);
    }

  asprintf (msg, "%s: %s\n", username, input);
}

// Description: todo
void *
handle_receive_message (void *arg)
{
  int sock_fd = *((int *)arg);
  int numbytes;
  char buf[MAXDATASIZE];

  while (1)
    {
      if ((numbytes = recv (sock_fd, buf, MAXDATASIZE - 1, 0)) == -1)
        {
          perror ("recv");
          pthread_exit (NULL);
        }

      buf[numbytes] = '\0';

      print_room (buf);
      fflush (stdout);

      memset (buf, 0, MAXDATASIZE);
    }
  return NULL;
}

void *
handle_input (void *arg)
{
  int sock_fd = *((int *)arg);
  char input[1024];
  char *formatted_msg = "";

  while (1)
    {
      if (fgets (input, sizeof (input), stdin) != NULL)
        {
          input[strcspn (input, "\n")] = '\0';
          msg_wuser (&formatted_msg, input);

          if (send (sock_fd, formatted_msg, strlen (formatted_msg), 0) == -1)
            {
              perror ("send");
              pthread_exit (NULL);
            }

          free (formatted_msg);
          formatted_msg = "";
        }
      else
        {
          perror ("fgets");
          pthread_exit (NULL);
        }

      memset (input, 0, sizeof (input));
    }

  return NULL;
}

struct addrinfo *
get_server_info ()
{
  struct addrinfo hints, *servinfo;
  int rv;

  memset (&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;      // use any address family
  hints.ai_socktype = SOCK_STREAM;  // use TCP

  if ((rv = getaddrinfo (ADDR, PORT, &hints, &servinfo)) == -1)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (rv));
      exit (1);
    }

  return servinfo;
}

void
network_to_string (char *s, const struct addrinfo *p)
{
  inet_ntop (p->ai_family, &((struct sockaddr_in *)p->ai_addr)->sin_addr, s,
             sizeof *s);
}

void
create_and_connect_socket (int *sock_fd)
{
  char s[INET6_ADDRSTRLEN];
  struct addrinfo *p;

  struct addrinfo *servinfo = get_server_info ();
  for (p = servinfo; p != NULL; p = p->ai_next)
    {
      if ((*sock_fd = socket (p->ai_family, p->ai_socktype, p->ai_protocol))
          == -1)
        {
          perror ("Client: socket");
          continue;
        }

      if (connect (*sock_fd, p->ai_addr, p->ai_addrlen) == -1)
        {
          close (*sock_fd);
          continue;
        }

      break;
    }

  if (p == NULL)
    {
      print_no_server ();

      exit (2);
    }

  // Success
  print_connected ();
  username = print_create_user ();

  network_to_string (s, p);
}

void
create_client ()
{
  print_loading ();

  sleep (2);

  int sock_fd;

  struct addrinfo *servinfo = get_server_info ();
  create_and_connect_socket (&sock_fd);

  freeaddrinfo (servinfo);

  // receive message
  int receive_fd = sock_fd;
  pthread_t receive_thread;
  if (pthread_create (&receive_thread, NULL, &handle_receive_message,
                      &receive_fd)
      != 0)
    {
      perror ("pthread_create");
      exit (1);
    }

  // handle user input and send
  int send_fd = sock_fd;
  pthread_t input_thread;
  if (pthread_create (&input_thread, NULL, &handle_input, (void *)&send_fd)
      != 0)
    {
      perror ("pthread_create");
      exit (1);
    }

  pthread_join (receive_thread, NULL);
  pthread_join (input_thread, NULL);

  // close the connection to the server
  close (sock_fd);
}

int
main (void)
{
  init ();

  create_client ();

  return EXIT_SUCCESS;
}