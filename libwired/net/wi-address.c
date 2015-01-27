/*
 *  Copyright (c) 2015 Axel Andersson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>

#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif

#include <wired/wi-address.h>
#include <wired/wi-compat.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>

#ifndef SA_LEN  
#ifdef HAVE_STRUCT_SOCKADDR_SA_LEN  
#define SA_LEN(sa)                      ((sa)->sa_len)
#else
static size_t sa_len(const struct sockaddr *sa) {  
    switch(sa->sa_family) {  
        case AF_INET:  
            return sizeof(struct sockaddr_in);
            break;
                     
        case AF_INET6:
            return sizeof(struct sockaddr_in6);
            break;

        default:
            return sizeof(struct sockaddr);
            break;
    }  
}
#define SA_LEN(sa)                      (sa_len(sa))
#endif
#endif


struct _wi_address {
    wi_runtime_base_t                   base;
    
    struct sockaddr_storage             ss;
};


static wi_runtime_instance_t *          _wi_address_copy(wi_runtime_instance_t *);
static wi_boolean_t                     _wi_address_is_equal(wi_runtime_instance_t *, wi_runtime_instance_t *);
static wi_string_t *                    _wi_address_description(wi_runtime_instance_t *);
static wi_hash_code_t                   _wi_address_hash(wi_runtime_instance_t *);

static wi_boolean_t                     _wi_address_ipv4_matches_wildcard(wi_address_t *, wi_string_t *);
static wi_boolean_t                     _wi_address_ipv4_matches_netmask(wi_address_t *, wi_string_t *);
static wi_boolean_t                     _wi_address_ipv4_matches_literal(wi_address_t *, wi_string_t *);
static wi_boolean_t                     _wi_address_ipv6_matches_literal(wi_address_t *, wi_string_t *);
static uint32_t                         _wi_address_ipv4_uint32(wi_string_t *);
static wi_string_t *                    _wi_address_ipv6_expanded_value(wi_string_t *);


static wi_runtime_id_t                  _wi_address_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_address_runtime_class = {
    "wi_address_t",
    NULL,
    _wi_address_copy,
    _wi_address_is_equal,
    _wi_address_description,
    _wi_address_hash
};



void wi_address_register(void) {
    _wi_address_runtime_id = wi_runtime_register_class(&_wi_address_runtime_class);
}



void wi_address_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_address_runtime_id(void) {
    return _wi_address_runtime_id;
}



#pragma mark -

wi_address_t * wi_address_with_sa(struct sockaddr *sa) {
    return wi_autorelease(wi_address_init_with_sa(wi_address_alloc(), sa));
}



wi_address_t * wi_address_with_wildcard_for_family(wi_address_family_t family) {
    return wi_autorelease(wi_address_init_with_wildcard_for_family(wi_address_alloc(), family));
}



wi_address_t * wi_address_with_string(wi_string_t *string) {
    return wi_autorelease(wi_address_init_with_string(wi_address_alloc(), string));
}



#pragma mark -

wi_address_t * wi_address_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_address_runtime_id, sizeof(wi_address_t), WI_RUNTIME_OPTION_IMMUTABLE);
}



wi_address_t * wi_mutable_address_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_address_runtime_id, sizeof(wi_address_t), WI_RUNTIME_OPTION_MUTABLE);
}



wi_address_t * wi_address_init_with_sa(wi_address_t *address, struct sockaddr *sa) {
    if(sa->sa_family != AF_INET && sa->sa_family != AF_INET6) {
        wi_error_set_error(WI_ERROR_DOMAIN_GAI, EAI_FAMILY);
        
        wi_release(address);
        
        return NULL;
    }
    
    memcpy(&address->ss, sa, SA_LEN(sa));

    return address;
}



wi_address_t * wi_address_init_with_wildcard_for_family(wi_address_t *address, wi_address_family_t family) {
    struct sockaddr_in      sa;
    struct sockaddr_in6     sa6;

    switch(family) {
        case WI_ADDRESS_IPV4:
            memset(&sa, 0, sizeof(sa));
            sa.sin_family       = AF_INET;
            sa.sin_addr.s_addr  = INADDR_ANY;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
            sa.sin_len          = sizeof(sa);
#endif

            return wi_address_init_with_sa(address, (struct sockaddr *) &sa);
            break;

        case WI_ADDRESS_IPV6:
            memset(&sa6, 0, sizeof(sa6));
            sa6.sin6_family     = AF_INET6;
            sa6.sin6_addr       = in6addr_any;
#ifdef HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN
            sa6.sin6_len        = sizeof(sa6);
#endif

            return wi_address_init_with_sa(address, (struct sockaddr *) &sa6);
            break;
    }

    return NULL;
}



wi_address_t * wi_address_init_with_string(wi_address_t *address, wi_string_t *string) {
    struct sockaddr_in      sa_in;
    struct sockaddr_in6     sa_in6;
    
    if(wi_string_contains_string(string, WI_STR("."), 0)) {
        memset(&sa_in, 0, sizeof(sa_in));
        sa_in.sin_family    = AF_INET;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
        sa_in.sin_len       = sizeof(sa_in);
#endif
        
        if(inet_pton(AF_INET, wi_string_utf8_string(string), &sa_in.sin_addr) > 0)
            return wi_address_init_with_sa(address, (struct sockaddr *) &sa_in);
    }
    else if(wi_string_contains_string(string, WI_STR(":"), 0)) {
        memset(&sa_in6, 0, sizeof(sa_in6));
        sa_in6.sin6_family  = AF_INET6;
#ifdef HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN
        sa_in6.sin6_len     = sizeof(sa_in6);
#endif
        
        if(inet_pton(AF_INET6, wi_string_utf8_string(string), &sa_in6.sin6_addr) > 0)
            return wi_address_init_with_sa(address, (struct sockaddr *) &sa_in6);
    }

    wi_release(address);
    
    wi_error_set_libwired_error(WI_ERROR_ADDRESS_INVALIDADDRESS);
    
    return NULL;
}



static wi_runtime_instance_t * _wi_address_copy(wi_runtime_instance_t *instance) {
    wi_address_t    *address = instance;
    
    return wi_address_init_with_sa(wi_address_alloc(), (struct sockaddr *) &address->ss);
}



static wi_boolean_t _wi_address_is_equal(wi_runtime_instance_t *instance1, wi_runtime_instance_t *instance2) {
    wi_address_t    *address1 = instance1;
    wi_address_t    *address2 = instance2;
    
    return wi_is_equal(wi_address_string(address1), wi_address_string(address2)) && wi_address_port(address1) == wi_address_port(address2);
}



static wi_string_t * _wi_address_description(wi_runtime_instance_t *instance) {
    wi_address_t    *address = instance;
    wi_string_t     *family;
    
    switch(wi_address_family(address)) {
        case WI_ADDRESS_IPV4:
            family = WI_STR("ipv4");
            break;

        case WI_ADDRESS_IPV6:
            family = WI_STR("ipv6");
            break;
    }
    
    return wi_string_with_format(WI_STR("<%@ %p>{family = %@, address = %@, port = %lu}"),
       wi_runtime_class_name(address),
       address,
       family,
       wi_address_string(address),
       wi_address_port(address));
}



static wi_hash_code_t _wi_address_hash(wi_runtime_instance_t *instance) {
    wi_address_t    *address = instance;
    
    return wi_hash(wi_address_string(address));
}



#pragma mark -

wi_integer_t wi_address_compare_family(wi_runtime_instance_t *instance1, wi_runtime_instance_t *instance2) {
    wi_address_t            *address1 = instance1;
    wi_address_t            *address2 = instance2;
    wi_address_family_t     family1;
    wi_address_family_t     family2;
    
    family1 = wi_address_family(address1);
    family2 = wi_address_family(address2);
    
    if(family1 == WI_ADDRESS_IPV4 && family2 == WI_ADDRESS_IPV6)
        return 1;
    else if(family1 == WI_ADDRESS_IPV6 && family2 == WI_ADDRESS_IPV4)
        return -1;
    
    return 0;
}



struct sockaddr * wi_address_sa(wi_address_t *address) {
    return (struct sockaddr *) &address->ss;
}



wi_uinteger_t wi_address_sa_length(wi_address_t *address) {
    return SA_LEN((struct sockaddr *) &address->ss);
}



wi_address_family_t wi_address_family(wi_address_t *address) {
    if(address->ss.ss_family == AF_INET)
        return WI_ADDRESS_IPV4;
    else
        return WI_ADDRESS_IPV6;
}



wi_uinteger_t wi_address_port(wi_address_t *address) {
    if(address->ss.ss_family == AF_INET)
        return ntohs(((struct sockaddr_in *) &address->ss)->sin_port);
    else
        return ntohs(((struct sockaddr_in6 *) &address->ss)->sin6_port);
}



#pragma mark -

wi_string_t * wi_address_string(wi_address_t *address) {
    char    string[NI_MAXHOST];
    int     err;
    
    err = getnameinfo(wi_address_sa(address), wi_address_sa_length(address), string, sizeof(string), NULL, 0, NI_NUMERICHOST);
    
    if(err != 0) {
        wi_error_set_error(WI_ERROR_DOMAIN_GAI, err);
        
        return NULL;
    }

    return wi_string_with_utf8_string(string);
}



wi_string_t * wi_address_hostname(wi_address_t *address) {
    char    string[NI_MAXHOST];
    int     err;
    
    err = getnameinfo(wi_address_sa(address), wi_address_sa_length(address), string, sizeof(string), NULL, 0, NI_NAMEREQD);
    
    if(err != 0) {
        wi_error_set_error(WI_ERROR_DOMAIN_GAI, err);
        
        return NULL;
    }

    return wi_string_with_utf8_string(string);
}



#pragma mark -

wi_boolean_t wi_address_matches_pattern(wi_address_t *address, wi_string_t *pattern) {
    if(address->ss.ss_family == AF_INET) {
        if(wi_string_contains_string(pattern, WI_STR("*"), 0))
            return _wi_address_ipv4_matches_wildcard(address, pattern);
        else if(wi_string_contains_string(pattern, WI_STR("/"), 0))
            return _wi_address_ipv4_matches_netmask(address, pattern);
        else
            return _wi_address_ipv4_matches_literal(address, pattern);
    } else {
        return _wi_address_ipv6_matches_literal(address, pattern);
    }
}



static wi_boolean_t _wi_address_ipv4_matches_wildcard(wi_address_t *address, wi_string_t *pattern) {
    wi_array_t      *ip_octets, *pattern_octets;
    wi_string_t     *ip, *ip_octet, *pattern_octet;
    wi_uinteger_t   i, count;
    
    count           = 0;
    ip              = wi_address_string(address);
    ip_octets       = wi_string_components_separated_by_string(ip, WI_STR("."));
    pattern_octets  = wi_string_components_separated_by_string(pattern, WI_STR("."));
    
    if(wi_array_count(ip_octets) != 4)
        return false;
    
    if(wi_array_count(ip_octets) != wi_array_count(pattern_octets))
        return false;
    
    for(i = 0; i < 4; i++) {
        ip_octet        = WI_ARRAY(ip_octets, i);
        pattern_octet   = WI_ARRAY(pattern_octets, i);
        
        if(wi_is_equal(ip_octet, pattern_octet) || wi_is_equal(pattern_octet, WI_STR("*")))
            count++;
    }
    
    return (count == 4);
}



static wi_boolean_t _wi_address_ipv4_matches_netmask(wi_address_t *address, wi_string_t *pattern) {
    wi_string_t     *ip, *pattern_ip, *pattern_netmask;
    wi_array_t      *array;
    uint32_t        cidr, netmask;
    
    array = wi_string_components_separated_by_string(pattern, WI_STR("/"));
    
    if(wi_array_count(array) != 2)
        return false;
    
    ip                  = wi_address_string(address);
    pattern_ip          = WI_ARRAY(array, 0);
    pattern_netmask     = WI_ARRAY(array, 1);
    
    if(wi_string_contains_string(pattern_netmask, WI_STR("."), 0)) {
        netmask = _wi_address_ipv4_uint32(pattern_netmask);
    } else {
        cidr = wi_string_uint32(pattern_netmask);
        netmask = pow(2.0, 32.0) - pow(2.0, 32.0 - cidr);
    }
    
    return ((_wi_address_ipv4_uint32(ip) & netmask) == (_wi_address_ipv4_uint32(pattern_ip) & netmask));
}



static wi_boolean_t _wi_address_ipv4_matches_literal(wi_address_t *address, wi_string_t *pattern) {
    return wi_is_equal(wi_address_string(address), pattern);
}



static wi_boolean_t _wi_address_ipv6_matches_literal(wi_address_t *address, wi_string_t *pattern) {
    wi_string_t     *ip, *ip_expanded, *pattern_expanded;
    
    ip                  = wi_address_string(address);
    ip_expanded         = _wi_address_ipv6_expanded_value(ip);
    pattern_expanded    = _wi_address_ipv6_expanded_value(pattern);
    
    return (ip_expanded && pattern_expanded && wi_is_equal(ip_expanded, pattern_expanded));
}



static uint32_t _wi_address_ipv4_uint32(wi_string_t *ip) {
    uint32_t    a, b, c, d;
    
    if(sscanf(wi_string_utf8_string(ip), "%u.%u.%u.%u", &a, &b, &c, &d) == 4)
        return (a << 24) + (b << 16) + (c << 8) + d;
    
    return 0;
}



static wi_string_t * _wi_address_ipv6_expanded_value(wi_string_t *ip) {
    wi_mutable_array_t      *octets;
    wi_mutable_string_t     *octet;
    wi_uinteger_t           i, count;
    
    octets  = wi_autorelease(wi_mutable_copy(wi_string_components_separated_by_string(ip, WI_STR(":"))));
    count   = wi_array_count(octets);
    
    if(count < 3)
        return NULL;
    
    while(count > 0 && wi_string_length(WI_ARRAY(octets, 0)) == 0) {
        wi_mutable_array_remove_data_at_index(octets, 0);
        count--;
    }
    
    for(i = 0; i < count; i++) {
        octet = wi_mutable_copy(WI_ARRAY(octets, i));
        
        while(wi_string_length(octet) < 4)
            wi_mutable_string_insert_string_at_index(octet, WI_STR("0"), 0);
        
        wi_mutable_array_replace_data_at_index(octets, octet, i);
        wi_release(octet);
    }
    
    while(count < 8) {
        if(count == 0)
            wi_mutable_array_add_data(octets, WI_STR("0000"));
        else
            wi_mutable_array_insert_data_at_index(octets, WI_STR("0000"), 0);
        
        count++;
    }
    
    return wi_array_components_joined_by_string(octets, WI_STR(":"));
}



#pragma mark -

wi_boolean_t wi_mutable_address_set_sa(wi_mutable_address_t *address, struct sockaddr *sa) {
    if(sa->sa_family != AF_INET && sa->sa_family != AF_INET6) {
        wi_error_set_error(WI_ERROR_DOMAIN_GAI, EAI_FAMILY);
        
        return false;
    }
    
    memcpy(&address->ss, sa, SA_LEN(sa));
    
    return true;
}



void wi_mutable_address_set_port(wi_mutable_address_t *address, wi_uinteger_t port) {
    if(address->ss.ss_family == AF_INET)
        ((struct sockaddr_in *) &address->ss)->sin_port = htons(port);
    else
        ((struct sockaddr_in6 *) &address->ss)->sin6_port = htons(port);
}
