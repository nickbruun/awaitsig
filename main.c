#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define ARG_LENGTH_MIN 2
#define ARG_LENGTH_MAX 6


typedef void (*sig_handler_t)(int);


static void exit_sig_handler(int sig)
{
    (void)sig;
    exit(EXIT_SUCCESS);
}


static void print_usage(char *exec_path)
{
    char *exec_name = strrchr(exec_path, '/');
    if (!exec_name)
        exec_name = exec_path;
    else
        ++exec_name;

    fprintf(stderr,
            "Usage: \n\
\n\
    %s [options] [signal...]\n\
\n\
Waits for the specified signals before exiting. If a signal is not\n\
specified, it will be ignored.\n\
\n\
Supported signals:\n\
\n\
    hup   Hang-up signal.\n\
    int   Interrupt signal.\n\
    pipe  Broken pipe signal.\n\
    term  Termination signal.\n\
    usr1  User-defined signal 1.\n\
    usr2  User-defined signal 2.\n\
\n\
Available options:\n\
\n\
    -?, -h, --help  Display this help text and exit.\n\
",
            exec_name);
}


static void print_invalid_argument(char *exec_path, char *arg)
{
    if (*arg == '-')
        fprintf(stderr, "Invalid option: %s\n\n", arg);
    else
        fprintf(stderr, "Invalid signal: %s\n\n", arg);

    print_usage(exec_path);
}


int main(int argc, char **argv)
{
    // Parse the signal handlers.
    sig_handler_t sigterm_handler = SIG_IGN;
    sig_handler_t sigint_handler = SIG_IGN;
    sig_handler_t sighup_handler = SIG_IGN;
    sig_handler_t sigpipe_handler = SIG_IGN;
    sig_handler_t sigusr1_handler = SIG_IGN;
    sig_handler_t sigusr2_handler = SIG_IGN;

    char arg_lower[ARG_LENGTH_MAX + 1];

    for (int argi = 1; argi < argc; ++argi)
    {
        char *arg = argv[argi];
        size_t arg_len = strlen(arg);

        if ((arg_len < ARG_LENGTH_MIN) ||
            (arg_len > ARG_LENGTH_MAX))
        {
            print_invalid_argument(argv[0], arg);
            return EXIT_FAILURE;
        }

        // Convert the argument to lower case.
        char *arg_pos = arg;
        char *arg_lower_pos = arg_lower;
        char pos;
        while ((pos = *arg_pos++))
            *arg_lower_pos++ = tolower(pos);
        *arg_lower_pos = 0;

        // Parse the argument.
        if ((strcmp(arg_lower, "-h") == 0) ||
            (strcmp(arg_lower, "-?") == 0) ||
            (strcmp(arg_lower, "--help") == 0))
        {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        else if (strcmp(arg_lower, "term") == 0)
            sigterm_handler = exit_sig_handler;
        else if (strcmp(arg_lower, "int") == 0)
            sigint_handler = exit_sig_handler;
        else if (strcmp(arg_lower, "hup") == 0)
            sighup_handler = exit_sig_handler;
        else if (strcmp(arg_lower, "pipe") == 0)
            sigpipe_handler = exit_sig_handler;
        else if (strcmp(arg_lower, "usr1") == 0)
            sigusr1_handler = exit_sig_handler;
        else if (strcmp(arg_lower, "usr2") == 0)
            sigusr2_handler = exit_sig_handler;
        else
        {
            print_invalid_argument(argv[0], arg);
            return EXIT_FAILURE;
        }
    }

    // Set up the signal handlers.
    sig_handler_t old_handler;

#define SET_UP_SIGNAL_HANDLER(_signal, _handler)                    \
    if ((old_handler = signal(_signal, _handler)) == SIG_ERR)       \
    {                                                               \
        fprintf(stderr, "Failed to set signal handler " #_signal);  \
        return EXIT_FAILURE;                                        \
    }

    SET_UP_SIGNAL_HANDLER(SIGTERM, sigterm_handler)
    SET_UP_SIGNAL_HANDLER(SIGINT, sigint_handler)
    SET_UP_SIGNAL_HANDLER(SIGHUP, sighup_handler)
    SET_UP_SIGNAL_HANDLER(SIGPIPE, sigpipe_handler)
    SET_UP_SIGNAL_HANDLER(SIGUSR1, sigusr1_handler)
    SET_UP_SIGNAL_HANDLER(SIGUSR2, sigusr2_handler)

#undef SET_UP_SIGNAL_HANDLER

    // Sleep indefinitely.
    while (1)
        sleep(3600);

    return EXIT_SUCCESS;
}
