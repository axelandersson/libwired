# config.m4

AC_DEFUN([_WI_MSG_LIB_ERROR], [
    AC_MSG_ERROR([could not locate $1

If you installed $1 into a non-standard directory, please run:

    env CPPFLAGS="-I/path/to/include" LDFLAGS="-L/path/to/lib" ./configure])
])


AC_DEFUN([WI_APPEND_FLAG], [
    if test -z "$$1"; then
        $1="$2"
    else
        MATCH=`expr -- "$$1" : ".*$2"`

        if test "$MATCH" = "0"; then
            $1="$$1 $2"
        fi
    fi
])


AC_DEFUN([WI_INCLUDE_WARNING_FLAG], [
    OLD_CFLAGS="$CFLAGS"
    WI_APPEND_FLAG([CFLAGS], $1)

    AC_COMPILE_IFELSE([AC_LANG_SOURCE], [
        WI_APPEND_FLAG([WARNFLAGS], $1)
    ], [
        CFLAGS="$OLD_CFLAGS"
    ])
])


AC_DEFUN([WI_INCLUDE_EXTRA_INCLUDE_PATHS], [
    if test "$wi_include_extra_include_paths_done" != "yes"; then
        if test -d /usr/local/include; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/local/include])
        fi

        if test -d /usr/pkg/include; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/pkg/include])
        fi

        wi_include_extra_include_paths_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_EXTRA_LIBRARY_PATHS], [
    if test "$wi_include_extra_library_paths_done" != "yes"; then
        if test -d /usr/local/lib; then
            WI_APPEND_FLAG([LDFLAGS], [-L/usr/local/lib])
        fi

        if test -d /usr/pkg/lib; then
            WI_APPEND_FLAG([LDFLAGS], [-L/usr/pkg/lib])
        fi

        if test -d /lib; then
            WI_APPEND_FLAG([LDFLAGS], [-L/lib])
        fi

        wi_include_extra_library_paths_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_EXTRA_SSL_PATHS], [
    if test "$wi_include_extra_ssl_paths_done" != "yes"; then
        if test -d /usr/local/ssl/include; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/local/ssl/include])
        fi

        if test -d /usr/kerberos/include; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/kerberos/include])
        fi

        if test -d /usr/local/ssl/lib; then
            WI_APPEND_FLAG([LDFLAGS], [-L/usr/local/ssl/lib])
        fi

        wi_include_extra_ssl_paths_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_LIBWIRED_LIBRARIES], [
    WI_INCLUDE_MATH_LIBRARY
    WI_INCLUDE_SOCKET_LIBRARY
    WI_INCLUDE_NSL_LIBRARY
    WI_INCLUDE_RESOLV_LIBRARY
    WI_INCLUDE_COMMON_CRYPTO_LIBRARIES
])


AC_DEFUN([WI_INCLUDE_OPENSSL_LIBRARIES], [
    WI_INCLUDE_CRYPTO_LIBRARY
    WI_INCLUDE_SSL_LIBRARY
])


AC_DEFUN([WI_INCLUDE_COMMON_CRYPTO_LIBRARIES], [
    AC_CHECK_HEADERS([ \
        CommonCrypto/CommonDigest.h \
        CommonCrypto/CommonCryptor.h \
    ], [
        WI_APPEND_FLAG([CPPFLAGS], [-DWI_MD5])
        WI_APPEND_FLAG([CPPFLAGS], [-DWI_SHA1])
        WI_APPEND_FLAG([CPPFLAGS], [-DWI_CIPHERS])
        m4_ifvaln([$1], [$1], [:])
    ], [
        m4_ifvaln([$2], [$2], [:])
    ])
])


AC_DEFUN([WI_INCLUDE_MATH_LIBRARY], [
    if test "$wi_include_math_library_done" != "yes"; then
        AC_CHECK_FUNC([pow], [], [
            AC_CHECK_LIB([m], [sqrt], [
                WI_APPEND_FLAG([LIBS], [-lm])
            ])
        ])

        wi_include_math_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_SOCKET_LIBRARY], [
    if test "$wi_include_socket_library_done" != "yes"; then
        AC_CHECK_FUNC(setsockopt, [], [
            AC_CHECK_LIB([socket], [setsockopt], [
                WI_APPEND_FLAG([LIBS], [-lsocket])
            ])
        ])

        wi_include_socket_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_NSL_LIBRARY], [
    if test "$wi_include_nsl_library_done" != "yes"; then
        AC_CHECK_FUNC([gethostent], [], [
            AC_CHECK_LIB([nsl], [gethostent], [
                WI_APPEND_FLAG([LIBS], [-lnsl])
            ])
        ])

        wi_include_nsl_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_RESOLV_LIBRARY], [
    if test "$wi_include_resolv_library_done" != "yes"; then
        AC_CHECK_FUNC([herror], [], [
            AC_CHECK_LIB([resolv], [herror], [
                WI_APPEND_FLAG([LIBS], [-lresolv])
            ])
        ])

        wi_include_resolv_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_CRYPTO_LIBRARY], [
    if test "$wi_include_crypto_library_done" != "yes"; then
        WI_INCLUDE_EXTRA_SSL_PATHS

        AC_CHECK_HEADERS([openssl/sha.h], [], [
            if test "$1" != "noerror"; then
                _WI_MSG_LIB_ERROR([OpenSSL])
            fi
        ])

        AC_CHECK_LIB([crypto], [SHA1_Init], [
            WI_APPEND_FLAG([LIBS], [-lcrypto])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_MD5])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_SHA1])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_CIPHERS])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_DH])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_RSA])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_DSA])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_X509])
        ], [
            if test "$1" != "noerror"; then
                _WI_MSG_LIB_ERROR([OpenSSL])
            fi
        ])

        wi_include_crypto_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_SSL_LIBRARY], [
    if test "$wi_include_ssl_library_done" != "yes"; then
        WI_INCLUDE_EXTRA_SSL_PATHS

        AC_CHECK_HEADERS([openssl/ssl.h], [], [
            if test "$1" != "noerror"; then
                _WI_MSG_LIB_ERROR([OpenSSL])
            fi
        ])

        AC_CHECK_LIB([ssl], [SSL_library_init], [
            WI_APPEND_FLAG([LIBS], [-lssl])
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_SSL])
        ], [
            if test "$1" != "noerror"; then
                _WI_MSG_LIB_ERROR([OpenSSL])
            fi
        ])

        wi_include_ssl_library_done="yes"
    fi
])


AC_DEFUN([_WI_PTHREAD_TEST_INCLUDES], [
    #include <pthread.h>
    #include <errno.h>

    void * thread(void *arg) {
        return NULL;
    }
])


AC_DEFUN([_WI_PTHREAD_TEST_FUNCTION], [
    pthread_t tid;

    if(pthread_create(&tid, 0, thread, NULL) < 0)
        return errno;

    return 0;
])


AC_DEFUN([_WI_PTHREAD_TEST_PROGRAM], [
    _WI_PTHREAD_TEST_INCLUDES

    int main(void) {
        _WI_PTHREAD_TEST_FUNCTION
    }
])


AC_DEFUN([_WI_PTHREAD_TRY], [
    if test "$wi_pthreads_found" != "yes"; then
        OLD_LIBS="$LIBS"
        WI_APPEND_FLAG([LIBS], $1)

        AC_RUN_IFELSE([AC_LANG_SOURCE([_WI_PTHREAD_TEST_PROGRAM])], [
            wi_pthreads_test=yes
        ], [
            wi_pthreads_test=no
        ], [
            AC_LINK_IFELSE([AC_LANG_PROGRAM([_WI_PTHREAD_TEST_INCLUDES], [_WI_PTHREAD_TEST_FUNCTION])], [
                wi_pthreads_test=yes
            ], [
                wi_pthreads_test=no
            ])
        ])

        LIBS="$OLD_LIBS"

        if test "$wi_pthreads_test" = "yes"; then
            wi_pthreads_found="yes"
            wi_pthreads_libs="$1"
        fi
    fi
])


AC_DEFUN([WI_INCLUDE_PTHREADS], [
    if test "$wi_include_pthreads_done" != "yes"; then
        case $host in
            *-solaris*)
                AC_DEFINE([_POSIX_PTHREAD_SEMANTICS], [], [Define on Solaris to get sigwait() to work using pthreads semantics.])
                ;;
        esac

        AC_CHECK_HEADERS([pthread.h], [
            AC_MSG_CHECKING([for pthreads])

            _WI_PTHREAD_TRY([])
            _WI_PTHREAD_TRY([-pthread])
            _WI_PTHREAD_TRY([-lpthread])

            if test "$wi_pthreads_found" = "yes"; then
                AC_MSG_RESULT([yes])
                WI_APPEND_FLAG([LIBS], $wi_pthreads_libs)
            else
                AC_MSG_RESULT([no])
                AC_MSG_ERROR([could not locate pthreads])
            fi
        ], [
            AC_MSG_ERROR([could not locate pthreads])
        ])

        AC_CHECK_FUNCS([pthread_setname_np pthread_getname_np])

        WI_APPEND_FLAG([CPPFLAGS], [-DWI_PTHREADS])

        wi_include_pthreads_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_ICONV_LIBRARY], [
    if test "$wi_include_iconv_library_done" != "yes"; then
        AC_CHECK_HEADERS([iconv.h], [], [
            _WI_MSG_LIB_ERROR([iconv])
        ])

        AC_CHECK_LIB([iconv], [iconv], [
            WI_APPEND_FLAG([LIBS], [-liconv])
        ], [
            AC_CHECK_LIB([iconv], [libiconv], [
                WI_APPEND_FLAG([LIBS], [-liconv])
            ], [
                AC_CHECK_FUNC([iconv], [], [
                    _WI_MSG_LIB_ERROR([iconv])
                ])
            ])
        ])

        AC_MSG_CHECKING([if iconv understands Unicode])
        AC_RUN_IFELSE([AC_LANG_SOURCE([
            #include <iconv.h>
            int main(void) {
                iconv_t conv = iconv_open("UTF-8", "UTF-16");
                if(conv == (iconv_t) -1)
                    return 1;
                return 0;
            }
        ])], [
            AC_MSG_RESULT([yes])
        ], [
            AC_MSG_ERROR([no])
        ])

        WI_APPEND_FLAG([CPPFLAGS], [-DWI_STRING_ENCODING])

        wi_include_iconv_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_LIBXML2_LIBRARY], [
    if test "$wi_include_libxml2_library_done" != "yes"; then
        if test -d /usr/include/libxml2; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/include/libxml2])
        fi

        if test -d /usr/local/include/libxml2; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/local/include/libxml2])
        fi

        if test -d /usr/pkg/include/libxml2; then
            WI_APPEND_FLAG([CPPFLAGS], [-I/usr/pkg/include/libxml2])
        fi

        AC_CHECK_HEADERS([libxml/parser.h], [], [
            _WI_MSG_LIB_ERROR([libxml2])
        ])

        AC_CHECK_LIB([xml2], [xmlParseFile], [
            WI_APPEND_FLAG([LIBS], [-lxml2])
        ], [
            _WI_MSG_LIB_ERROR([libxml2])
        ])

        WI_APPEND_FLAG([CPPFLAGS], [-DWI_XML])
        WI_APPEND_FLAG([CPPFLAGS], [-DWI_PLIST])

        wi_include_libxml2_library_done="yes"
    fi
])


AC_DEFUN([WI_INCLUDE_FILESYSTEM_EVENTS], [
    if test "$wi_include_filesystem_events_done" != "yes" -a "$wi_pthreads_found" = "yes"; then
        AC_CHECK_HEADERS([sys/event.h], [
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_FILESYSTEM_EVENTS])
        ], [], [
            #include <sys/types.h>
        ])

        AC_CHECK_HEADERS([ \
            sys/inotify.h \
            inotifytools/inotify.h \
        ], [
            WI_APPEND_FLAG([CPPFLAGS], [-DWI_FILESYSTEM_EVENTS])
        ], [], [
            #include <sys/types.h>
        ])

        wi_include_filesystem_events_done="yes"
    fi
])
