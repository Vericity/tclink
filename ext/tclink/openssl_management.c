/**
 * OpenSSL uses many data structures on which operations must be atomic.
 * OpenSSL provides for the thread safety of its data structures by requiring
 * each thread to acquire a mutually exclusive lock known as a mutex that
 * protects the structure before allowing it to be accessed.
 * When the thread is finished with the data structure, it releases the mutex,
 * allowing another thread to acquire the lock and access the data structure.
 * OpenSSL requires the application programmer to perform these operations in
 * a manner appropriate for the platform it's running on by making callbacks to
 * functions that the application registers with OpenSSL for this purpose.
 *
 * Use following link for additional details:
 * 'https://www.openssl.org/docs/man1.0.1/crypto/threads.html'
 * 'https://wiki.openssl.org/index.php/Library_Initialization'

 * Following code defines and utilizes global mutex array required to make
 * OpenSSL thread safe. It was adapted from an example
 * 'crypto/threads/mttest.c' provided with the OpenSSL package.
 *
 */

#include <assert.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#if OPENSSL_VERSION_NUMBER < 0x10100000L
#include <pthread.h>
#endif

void __attribute__((constructor)) TCLink_OpenSSLInit(void);
void __attribute__((destructor)) TCLink_OpenSSLCleanup(void);

#if OPENSSL_VERSION_NUMBER < 0x10100000L
static pthread_mutex_t* lock_cs;
static long* lock_count;

static void
TCLink_pthreads_thread_id(CRYPTO_THREADID* tid)
{
  CRYPTO_THREADID_set_numeric(tid, (unsigned long)pthread_self());
}

static void
TCLink_pthreads_locking_callback(int mode, int type, const char* file, int line)
{
  if (mode & CRYPTO_LOCK) {
    pthread_mutex_lock(&(lock_cs[type]));
    lock_count[type]++;
  } else {
    pthread_mutex_unlock(&(lock_cs[type]));
  }
}

static void
TCLink_thread_setup(void)
{
  int i = 0;
  lock_cs = OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
  lock_count = OPENSSL_malloc(CRYPTO_num_locks() * sizeof(long));

  if (!lock_cs || !lock_count) {
    /* Nothing we can do about this...void function! */
    if (lock_cs) {
      OPENSSL_free(lock_cs);
      lock_cs = NULL;
    }

    if (lock_count) {
      OPENSSL_free(lock_count);
      lock_count = NULL;
    }

    return;
  }

  for (i = 0; i < CRYPTO_num_locks(); i++) {
    lock_count[i] = 0;
    pthread_mutex_init(&(lock_cs[i]), NULL);
  }

  CRYPTO_THREADID_set_callback(TCLink_pthreads_thread_id);
  CRYPTO_set_locking_callback(TCLink_pthreads_locking_callback);
}

static void
TCLink_thread_cleanup(void)
{
  int i = 0;

  if (!lock_cs || !lock_count)
    return;

  CRYPTO_set_locking_callback(NULL);
  for (i = 0; i < CRYPTO_num_locks(); i++) {
    pthread_mutex_destroy(&(lock_cs[i]));
  }

  OPENSSL_free(lock_cs);
  OPENSSL_free(lock_count);
}

#endif
/**
 *
 * Initialize the OpenSSL library.
 * Also sets up static callback functions required for multi-thread safety.
 */
void
TCLink_OpenSSLInit(void)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  SSL_load_error_strings();
  assert(SSL_library_init() == 1);
  TCLink_thread_setup();
#else
  assert(OPENSSL_init_ssl(OPENSSL_INIT_LOAD_CRYPTO_STRINGS |
                            OPENSSL_INIT_ADD_ALL_CIPHERS |
                            OPENSSL_INIT_ADD_ALL_DIGESTS |
                            OPENSSL_INIT_LOAD_CONFIG | OPENSSL_INIT_ASYNC |
#ifdef OPENSSL_INIT_NO_ATEXIT
                            OPENSSL_INIT_NO_ATEXIT |
#endif
#ifdef OPENSSL_INIT_ATFORK
                            OPENSSL_INIT_ATFORK |
#endif
                            OPENSSL_INIT_LOAD_SSL_STRINGS,
                          NULL) == 1);
#endif
}

/**
 *
 * De-initializes the OpenSSL library.
 * Performs cleanup required for global data structures.
 */
void
TCLink_OpenSSLCleanup(void)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
  TCLink_thread_cleanup();
  EVP_cleanup();
  CRYPTO_cleanup_all_ex_data();
  ERR_free_strings();
#else
  OPENSSL_cleanup();
#endif
}
