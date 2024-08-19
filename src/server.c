//   (C) Thomas Baumeister, 2024  : "4
//   For more information, see the public domain dedication at the end of this
//   file.

#include "server.h"

void *
to_sockaddr_in (struct sockaddr *sa)
{
  return &(((struct sockaddr_in *)sa)->sin_addr);
}

void
sigchld_handler (int s)
{
  int saved_errno = errno;

  while (waitpid (-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

void
set_sock_opt (int sock_fd)
{
  int yes = 1;

  if (setsockopt (sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
    {
      perror ("setsocketopt");
      exit (1);
    }
}

void
open_bind_socket (int *sock_fd)
{
  struct addrinfo hints, *res;

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo (ADDR, PORT, &hints, &res);
  *sock_fd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);

  set_sock_opt (*sock_fd);

  bind (*sock_fd, res->ai_addr, res->ai_addrlen);

  free (res);
}

int
get_listener (int sock_fd)
{
  listen (sock_fd, BACKLOG);

  return sock_fd;
}
void
add_to_pfds (Poll_Info *pi, int new_fd)
{
  if (pi->fd_count == pi->fd_size)
    {
      pi->fd_size *= 2;

      pi->pfds = realloc (pi->pfds, sizeof (*pi->pfds) * (pi->fd_size));
    }

  (pi->pfds)[pi->fd_count].fd = new_fd;
  (pi->pfds)[pi->fd_count].events = POLLIN;

  (pi->fd_count)++;
}

void
del_from_pfds (struct pollfd pfds[], int i, int *fd_count)
{
  pfds[i] = pfds[*fd_count - 1];

  (*fd_count)--;
}

// Ash Williams is the sigchld_handler and will clean up zombies
void
ash_williams ()
{
  struct sigaction sa;

  printf ("Ash Williams: Does that sound fine?\n");

  sa.sa_handler = sigchld_handler;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction (SIGCHLD, &sa, NULL) == -1)
    {
      perror ("sigaction");
      exit (1);
    }
}

int
send_all (int fd, char *buf, int *len)
{
  int total_sent = 0;
  int bytes_left = *len;
  int n;

  while (total_sent < *len)
    {
      if ((n = send (fd, buf + total_sent, bytes_left, 0)) == -1)
        {
          break;
        }

      printf ("Server: sent %d bytes\n", n);

      total_sent += n;
      bytes_left -= n;
    }

  *len = total_sent;

  return n == -1 ? -1 : 0;
}

// TODO: Send from Client
void
broadcast_message (Poll_Info pi, int listener, int sender_fd, int nbytes,
                   char *buf)
{
  printf ("Debug: Buffer= %s", buf);
  printf ("Debug: nbytes=%d", nbytes);

  for (int j = 0; j < pi.fd_count; j++)
    {
      int dst_fd = pi.pfds[j].fd;

      // Except the listener and ourselves
      if (dst_fd != listener)
        {
          printf ("Debug: nbytes: %d\n", nbytes);
          printf ("Debug: buf: %s\n", buf);
          if (send (dst_fd, buf, nbytes, 0) == -1)
            {
              perror ("send");
            }

          /*
          if (send_all(dst_fd, buf, &nbytes) == -1) {
              perror("send");
          }
          */
        }
    }
}

void
handle_new_connection (Poll_Info *pi, int listener)
{
  struct sockaddr_storage client_addr;
  char client_ip[INET_ADDRSTRLEN];
  socklen_t addr_len = sizeof (client_addr);

  int new_fd = accept (listener, (struct sockaddr *)&client_addr, &addr_len);
  if (new_fd == -1)
    {
      perror ("accept");
      return;
    }
  else
    {
      printf ("Debug: (Before) fd_count=%d, fd_size=%d\n", pi->fd_count,
              pi->fd_size);
      add_to_pfds (pi, new_fd);

      printf ("Debug: (After) fd_count=%d, fd_size=%d\n", pi->fd_count,
              pi->fd_size);

      // Log the new connection
      printf ("Pollserver: new connection from %s on socket %d\n",
              inet_ntop (client_addr.ss_family,
                         to_sockaddr_in ((struct sockaddr *)&client_addr),
                         client_ip, sizeof (client_ip)),
              new_fd);

      // Send a welcome message
      if (send (new_fd, "Welcome!", 8, 0) == -1)
        {
          perror ("send");
        }
    }
}

// TODO: BIG TODO: This is a mess, needs to be cleaned up
void
on_poll (int listener, Poll_Info *pi, int index)
{
  char buf[256];

  printf ("Debug: index=%d, fd=%d\n", index, pi->pfds[index].fd);

  if (pi->pfds[index].fd == listener)
    { // Is our listener
      // Handle new connection
      printf ("Debug: Handling new connection\n");
      handle_new_connection (pi, listener);
    }
  else
    {
      printf ("Debug: Got data from client\n");
      printf ("Debug: Buffer: %s", buf);

      memset (buf, 0, sizeof (buf));

      // Receive Data from Client
      int nbytes = recv (pi->pfds[index].fd, buf, sizeof (buf), 0);
      int sender_fd = pi->pfds[index].fd;

      if (nbytes <= 0)
        {
          // Handle error or client disconnection
          if (nbytes == 0)
            {
              printf ("Pollserver: socket %d hung up\n", sender_fd);
            }
          else
            {
              perror ("recv");
			}

          close (sender_fd);
          del_from_pfds (pi->pfds, index, &pi->fd_count);
        }
      else
        { // We got data from client
          printf ("Debug: Broadcasting message\n");
          broadcast_message (*pi, listener, sender_fd, nbytes, buf);
        }
    } // END handle data from poll

  printf ("Debug: Exiting on_poll\n");
}

void
init_poll_info (Poll_Info *pi)
{
  pi->fd_count = 0;
  pi->fd_size = FD_START_SIZE;
  pi->pfds = malloc (sizeof *pi->pfds * pi->fd_size);
}

void
create_poll (Poll_Info pi)
{
  int poll_count = poll (pi.pfds, pi.fd_count, -1);
  printf ("Debug: count=%d\n", pi.fd_count);
  if (poll_count == -1)
    {
      perror ("poll");
      exit (1);
    }
}

void
check_for_event (Poll_Info *poll_info, int sock_fd, int listener)
{
  for (int i = 0; i < poll_info->fd_count; i++)
    { // Loop through all file_descriptors
      if (poll_info->pfds[i].revents & POLLIN)
        { // We got an event
          on_poll (listener, poll_info, i);
        } // END got an event
    } // END looping through file descriptors
}

int
create_server ()
{
  Poll_Info poll_info;
  init_poll_info (&poll_info);

  int sock_fd;
  int listener;

  open_bind_socket (&sock_fd);
  listener = get_listener (sock_fd);
  ash_williams ();

  poll_info.pfds[0].fd = listener;
  poll_info.pfds[0].events = POLLIN;
  poll_info.fd_count = 1;

  printf ("Server: Waiting for connections on: %s ...\n", ADDR);

  for (;;)
    {
      create_poll (poll_info);
      check_for_event (&poll_info, sock_fd, listener);
    } // END for(;;)
}

int
main (void)
{
  return create_server ();
}

// This is free and unencumbered software released into the public domain.

// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.

// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

// For more information, please refer to <http://unlicense.org/>
