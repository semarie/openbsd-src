/* $OpenBSD: ssl_clnt.c,v 1.117 2021/10/25 10:01:46 jsing Exp $ */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 *
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 *
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 *
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
/* ====================================================================
 * Copyright (c) 1998-2007 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 *
 * Portions of the attached software ("Contribution") are developed by
 * SUN MICROSYSTEMS, INC., and are contributed to the OpenSSL project.
 *
 * The Contribution is licensed pursuant to the OpenSSL open source
 * license provided above.
 *
 * ECC cipher suite support in OpenSSL originally written by
 * Vipul Gupta and Sumit Gupta of Sun Microsystems Laboratories.
 *
 */
/* ====================================================================
 * Copyright 2005 Nokia. All rights reserved.
 *
 * The portions of the attached software ("Contribution") is developed by
 * Nokia Corporation and is licensed pursuant to the OpenSSL open source
 * license.
 *
 * The Contribution, originally written by Mika Kousa and Pasi Eronen of
 * Nokia Corporation, consists of the "PSK" (Pre-Shared Key) ciphersuites
 * support (see RFC 4279) to OpenSSL.
 *
 * No patent licenses or other rights except those expressly stated in
 * the OpenSSL open source license shall be deemed granted or received
 * expressly, by implication, estoppel, or otherwise.
 *
 * No assurances are provided by Nokia that the Contribution does not
 * infringe the patent or other intellectual property rights of any third
 * party or that the license provides you with all the necessary rights
 * to make use of the Contribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. IN
 * ADDITION TO THE DISCLAIMERS INCLUDED IN THE LICENSE, NOKIA
 * SPECIFICALLY DISCLAIMS ANY LIABILITY FOR CLAIMS BROUGHT BY YOU OR ANY
 * OTHER ENTITY BASED ON INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS OR
 * OTHERWISE.
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include <openssl/bn.h>
#include <openssl/buffer.h>
#include <openssl/curve25519.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/objects.h>
#include <openssl/opensslconf.h>

#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif
#ifndef OPENSSL_NO_GOST
#include <openssl/gost.h>
#endif

#include "bytestring.h"
#include "dtls_locl.h"
#include "ssl_locl.h"
#include "ssl_sigalgs.h"
#include "ssl_tlsext.h"

static int ca_dn_cmp(const X509_NAME * const *a, const X509_NAME * const *b);

int
ssl3_connect(SSL *s)
{
	int new_state, state, skip = 0;
	int ret = -1;

	ERR_clear_error();
	errno = 0;

	s->internal->in_handshake++;
	if (!SSL_in_init(s) || SSL_in_before(s))
		SSL_clear(s);

	for (;;) {
		state = S3I(s)->hs.state;

		switch (S3I(s)->hs.state) {
		case SSL_ST_RENEGOTIATE:
			s->internal->renegotiate = 1;
			S3I(s)->hs.state = SSL_ST_CONNECT;
			s->ctx->internal->stats.sess_connect_renegotiate++;
			/* break */
		case SSL_ST_BEFORE:
		case SSL_ST_CONNECT:
		case SSL_ST_BEFORE|SSL_ST_CONNECT:
		case SSL_ST_OK|SSL_ST_CONNECT:

			s->server = 0;

			ssl_info_callback(s, SSL_CB_HANDSHAKE_START, 1);

			if (!ssl_legacy_stack_version(s, s->version)) {
				SSLerror(s, ERR_R_INTERNAL_ERROR);
				ret = -1;
				goto end;
			}

			if (!ssl_supported_tls_version_range(s,
			    &S3I(s)->hs.our_min_tls_version,
			    &S3I(s)->hs.our_max_tls_version)) {
				SSLerror(s, SSL_R_NO_PROTOCOLS_AVAILABLE);
				ret = -1;
				goto end;
			}

			if (!ssl3_setup_init_buffer(s)) {
				ret = -1;
				goto end;
			}
			if (!ssl3_setup_buffers(s)) {
				ret = -1;
				goto end;
			}
			if (!ssl_init_wbio_buffer(s, 0)) {
				ret = -1;
				goto end;
			}

			/* don't push the buffering BIO quite yet */

			if (!tls1_transcript_init(s)) {
				ret = -1;
				goto end;
			}

			S3I(s)->hs.state = SSL3_ST_CW_CLNT_HELLO_A;
			s->ctx->internal->stats.sess_connect++;
			s->internal->init_num = 0;

			if (SSL_is_dtls(s)) {
				/* mark client_random uninitialized */
				memset(s->s3->client_random, 0,
				    sizeof(s->s3->client_random));
				s->d1->send_cookie = 0;
				s->internal->hit = 0;
			}
			break;

		case SSL3_ST_CW_CLNT_HELLO_A:
		case SSL3_ST_CW_CLNT_HELLO_B:
			s->internal->shutdown = 0;

			if (SSL_is_dtls(s)) {
				/* every DTLS ClientHello resets Finished MAC */
				tls1_transcript_reset(s);

				dtls1_start_timer(s);
			}

			ret = ssl3_send_client_hello(s);
			if (ret <= 0)
				goto end;

			if (SSL_is_dtls(s) && s->d1->send_cookie) {
				S3I(s)->hs.state = SSL3_ST_CW_FLUSH;
				S3I(s)->hs.tls12.next_state = SSL3_ST_CR_SRVR_HELLO_A;
			} else
				S3I(s)->hs.state = SSL3_ST_CR_SRVR_HELLO_A;

			s->internal->init_num = 0;

			/* turn on buffering for the next lot of output */
			if (s->bbio != s->wbio)
				s->wbio = BIO_push(s->bbio, s->wbio);

			break;

		case SSL3_ST_CR_SRVR_HELLO_A:
		case SSL3_ST_CR_SRVR_HELLO_B:
			ret = ssl3_get_server_hello(s);
			if (ret <= 0)
				goto end;

			if (s->internal->hit) {
				S3I(s)->hs.state = SSL3_ST_CR_FINISHED_A;
				if (!SSL_is_dtls(s)) {
					if (s->internal->tlsext_ticket_expected) {
						/* receive renewed session ticket */
						S3I(s)->hs.state = SSL3_ST_CR_SESSION_TICKET_A;
					}

					/* No client certificate verification. */
					tls1_transcript_free(s);
				}
			} else if (SSL_is_dtls(s)) {
				S3I(s)->hs.state = DTLS1_ST_CR_HELLO_VERIFY_REQUEST_A;
			} else {
				S3I(s)->hs.state = SSL3_ST_CR_CERT_A;
			}
			s->internal->init_num = 0;
			break;

		case DTLS1_ST_CR_HELLO_VERIFY_REQUEST_A:
		case DTLS1_ST_CR_HELLO_VERIFY_REQUEST_B:
			ret = ssl3_get_dtls_hello_verify(s);
			if (ret <= 0)
				goto end;
			dtls1_stop_timer(s);
			if (s->d1->send_cookie) /* start again, with a cookie */
				S3I(s)->hs.state = SSL3_ST_CW_CLNT_HELLO_A;
			else
				S3I(s)->hs.state = SSL3_ST_CR_CERT_A;
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CR_CERT_A:
		case SSL3_ST_CR_CERT_B:
			ret = ssl3_check_finished(s);
			if (ret <= 0)
				goto end;
			if (ret == 2) {
				s->internal->hit = 1;
				if (s->internal->tlsext_ticket_expected)
					S3I(s)->hs.state = SSL3_ST_CR_SESSION_TICKET_A;
				else
					S3I(s)->hs.state = SSL3_ST_CR_FINISHED_A;
				s->internal->init_num = 0;
				break;
			}
			/* Check if it is anon DH/ECDH. */
			if (!(S3I(s)->hs.cipher->algorithm_auth &
			    SSL_aNULL)) {
				ret = ssl3_get_server_certificate(s);
				if (ret <= 0)
					goto end;
				if (s->internal->tlsext_status_expected)
					S3I(s)->hs.state = SSL3_ST_CR_CERT_STATUS_A;
				else
					S3I(s)->hs.state = SSL3_ST_CR_KEY_EXCH_A;
			} else {
				skip = 1;
				S3I(s)->hs.state = SSL3_ST_CR_KEY_EXCH_A;
			}
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CR_KEY_EXCH_A:
		case SSL3_ST_CR_KEY_EXCH_B:
			ret = ssl3_get_server_key_exchange(s);
			if (ret <= 0)
				goto end;
			S3I(s)->hs.state = SSL3_ST_CR_CERT_REQ_A;
			s->internal->init_num = 0;

			/*
			 * At this point we check that we have the
			 * required stuff from the server.
			 */
			if (!ssl3_check_cert_and_algorithm(s)) {
				ret = -1;
				goto end;
			}
			break;

		case SSL3_ST_CR_CERT_REQ_A:
		case SSL3_ST_CR_CERT_REQ_B:
			ret = ssl3_get_certificate_request(s);
			if (ret <= 0)
				goto end;
			S3I(s)->hs.state = SSL3_ST_CR_SRVR_DONE_A;
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CR_SRVR_DONE_A:
		case SSL3_ST_CR_SRVR_DONE_B:
			ret = ssl3_get_server_done(s);
			if (ret <= 0)
				goto end;
			if (SSL_is_dtls(s))
				dtls1_stop_timer(s);
			if (S3I(s)->hs.tls12.cert_request)
				S3I(s)->hs.state = SSL3_ST_CW_CERT_A;
			else
				S3I(s)->hs.state = SSL3_ST_CW_KEY_EXCH_A;
			s->internal->init_num = 0;

			break;

		case SSL3_ST_CW_CERT_A:
		case SSL3_ST_CW_CERT_B:
		case SSL3_ST_CW_CERT_C:
		case SSL3_ST_CW_CERT_D:
			if (SSL_is_dtls(s))
				dtls1_start_timer(s);
			ret = ssl3_send_client_certificate(s);
			if (ret <= 0)
				goto end;
			S3I(s)->hs.state = SSL3_ST_CW_KEY_EXCH_A;
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CW_KEY_EXCH_A:
		case SSL3_ST_CW_KEY_EXCH_B:
			if (SSL_is_dtls(s))
				dtls1_start_timer(s);
			ret = ssl3_send_client_key_exchange(s);
			if (ret <= 0)
				goto end;
			/*
			 * EAY EAY EAY need to check for DH fix cert
			 * sent back
			 */
			/*
			 * For TLS, cert_req is set to 2, so a cert chain
			 * of nothing is sent, but no verify packet is sent
			 */
			/*
			 * XXX: For now, we do not support client
			 * authentication in ECDH cipher suites with
			 * ECDH (rather than ECDSA) certificates.
			 * We need to skip the certificate verify
			 * message when client's ECDH public key is sent
			 * inside the client certificate.
			 */
			if (S3I(s)->hs.tls12.cert_request == 1) {
				S3I(s)->hs.state = SSL3_ST_CW_CERT_VRFY_A;
			} else {
				S3I(s)->hs.state = SSL3_ST_CW_CHANGE_A;
				S3I(s)->change_cipher_spec = 0;
			}
			if (!SSL_is_dtls(s)) {
				if (s->s3->flags & TLS1_FLAGS_SKIP_CERT_VERIFY) {
					S3I(s)->hs.state = SSL3_ST_CW_CHANGE_A;
					S3I(s)->change_cipher_spec = 0;
				}
			}

			s->internal->init_num = 0;
			break;

		case SSL3_ST_CW_CERT_VRFY_A:
		case SSL3_ST_CW_CERT_VRFY_B:
			if (SSL_is_dtls(s))
				dtls1_start_timer(s);
			ret = ssl3_send_client_verify(s);
			if (ret <= 0)
				goto end;
			S3I(s)->hs.state = SSL3_ST_CW_CHANGE_A;
			s->internal->init_num = 0;
			S3I(s)->change_cipher_spec = 0;
			break;

		case SSL3_ST_CW_CHANGE_A:
		case SSL3_ST_CW_CHANGE_B:
			if (SSL_is_dtls(s) && !s->internal->hit)
				dtls1_start_timer(s);
			ret = ssl3_send_change_cipher_spec(s,
			    SSL3_ST_CW_CHANGE_A, SSL3_ST_CW_CHANGE_B);
			if (ret <= 0)
				goto end;

			S3I(s)->hs.state = SSL3_ST_CW_FINISHED_A;
			s->internal->init_num = 0;
			s->session->cipher = S3I(s)->hs.cipher;

			if (!tls1_setup_key_block(s)) {
				ret = -1;
				goto end;
			}
			if (!tls1_change_write_cipher_state(s)) {
				ret = -1;
				goto end;
			}
			break;

		case SSL3_ST_CW_FINISHED_A:
		case SSL3_ST_CW_FINISHED_B:
			if (SSL_is_dtls(s) && !s->internal->hit)
				dtls1_start_timer(s);
			ret = ssl3_send_finished(s, SSL3_ST_CW_FINISHED_A,
			    SSL3_ST_CW_FINISHED_B);
			if (ret <= 0)
				goto end;
			if (!SSL_is_dtls(s))
				s->s3->flags |= SSL3_FLAGS_CCS_OK;
			S3I(s)->hs.state = SSL3_ST_CW_FLUSH;

			/* clear flags */
			if (s->internal->hit) {
				S3I(s)->hs.tls12.next_state = SSL_ST_OK;
			} else {
				/* Allow NewSessionTicket if ticket expected */
				if (s->internal->tlsext_ticket_expected)
					S3I(s)->hs.tls12.next_state =
					    SSL3_ST_CR_SESSION_TICKET_A;
				else
					S3I(s)->hs.tls12.next_state =
					    SSL3_ST_CR_FINISHED_A;
			}
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CR_SESSION_TICKET_A:
		case SSL3_ST_CR_SESSION_TICKET_B:
			ret = ssl3_get_new_session_ticket(s);
			if (ret <= 0)
				goto end;
			S3I(s)->hs.state = SSL3_ST_CR_FINISHED_A;
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CR_CERT_STATUS_A:
		case SSL3_ST_CR_CERT_STATUS_B:
			ret = ssl3_get_cert_status(s);
			if (ret <= 0)
				goto end;
			S3I(s)->hs.state = SSL3_ST_CR_KEY_EXCH_A;
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CR_FINISHED_A:
		case SSL3_ST_CR_FINISHED_B:
			if (SSL_is_dtls(s))
				s->d1->change_cipher_spec_ok = 1;
			else
				s->s3->flags |= SSL3_FLAGS_CCS_OK;
			ret = ssl3_get_finished(s, SSL3_ST_CR_FINISHED_A,
			    SSL3_ST_CR_FINISHED_B);
			if (ret <= 0)
				goto end;
			if (SSL_is_dtls(s))
				dtls1_stop_timer(s);

			if (s->internal->hit)
				S3I(s)->hs.state = SSL3_ST_CW_CHANGE_A;
			else
				S3I(s)->hs.state = SSL_ST_OK;
			s->internal->init_num = 0;
			break;

		case SSL3_ST_CW_FLUSH:
			s->internal->rwstate = SSL_WRITING;
			if (BIO_flush(s->wbio) <= 0) {
				if (SSL_is_dtls(s)) {
					/* If the write error was fatal, stop trying */
					if (!BIO_should_retry(s->wbio)) {
						s->internal->rwstate = SSL_NOTHING;
						S3I(s)->hs.state = S3I(s)->hs.tls12.next_state;
					}
				}
				ret = -1;
				goto end;
			}
			s->internal->rwstate = SSL_NOTHING;
			S3I(s)->hs.state = S3I(s)->hs.tls12.next_state;
			break;

		case SSL_ST_OK:
			/* clean a few things up */
			tls1_cleanup_key_block(s);

			if (S3I(s)->handshake_transcript != NULL) {
				SSLerror(s, ERR_R_INTERNAL_ERROR);
				ret = -1;
				goto end;
			}

			if (!SSL_is_dtls(s))
				ssl3_release_init_buffer(s);

			ssl_free_wbio_buffer(s);

			s->internal->init_num = 0;
			s->internal->renegotiate = 0;
			s->internal->new_session = 0;

			ssl_update_cache(s, SSL_SESS_CACHE_CLIENT);
			if (s->internal->hit)
				s->ctx->internal->stats.sess_hit++;

			ret = 1;
			/* s->server=0; */
			s->internal->handshake_func = ssl3_connect;
			s->ctx->internal->stats.sess_connect_good++;

			ssl_info_callback(s, SSL_CB_HANDSHAKE_DONE, 1);

			if (SSL_is_dtls(s)) {
				/* done with handshaking */
				s->d1->handshake_read_seq = 0;
				s->d1->next_handshake_write_seq = 0;
			}

			goto end;
			/* break; */

		default:
			SSLerror(s, SSL_R_UNKNOWN_STATE);
			ret = -1;
			goto end;
			/* break; */
		}

		/* did we do anything */
		if (!S3I(s)->hs.tls12.reuse_message && !skip) {
			if (s->internal->debug) {
				if ((ret = BIO_flush(s->wbio)) <= 0)
					goto end;
			}

			if (S3I(s)->hs.state != state) {
				new_state = S3I(s)->hs.state;
				S3I(s)->hs.state = state;
				ssl_info_callback(s, SSL_CB_CONNECT_LOOP, 1);
				S3I(s)->hs.state = new_state;
			}
		}
		skip = 0;
	}

 end:
	s->internal->in_handshake--;
	ssl_info_callback(s, SSL_CB_CONNECT_EXIT, ret);

	return (ret);
}

int
ssl3_send_client_hello(SSL *s)
{
	CBB cbb, client_hello, session_id, cookie, cipher_suites;
	CBB compression_methods;
	uint16_t max_version;
	size_t sl;

	memset(&cbb, 0, sizeof(cbb));

	if (S3I(s)->hs.state == SSL3_ST_CW_CLNT_HELLO_A) {
		SSL_SESSION *sess = s->session;

		if (!ssl_max_supported_version(s, &max_version)) {
			SSLerror(s, SSL_R_NO_PROTOCOLS_AVAILABLE);
			return (-1);
		}
		s->version = max_version;

		if (sess == NULL ||
		    sess->ssl_version != s->version ||
		    (!sess->session_id_length && !sess->tlsext_tick) ||
		    sess->not_resumable) {
			if (!ssl_get_new_session(s, 0))
				goto err;
		}
		/* else use the pre-loaded session */

		/*
		 * If a DTLS ClientHello message is being resent after a
		 * HelloVerifyRequest, we must retain the original client
		 * random value.
		 */
		if (!SSL_is_dtls(s) || s->d1->send_cookie == 0)
			arc4random_buf(s->s3->client_random, SSL3_RANDOM_SIZE);

		if (!ssl3_handshake_msg_start(s, &cbb, &client_hello,
		    SSL3_MT_CLIENT_HELLO))
			goto err;

		if (!CBB_add_u16(&client_hello, s->version))
			goto err;

		/* Random stuff */
		if (!CBB_add_bytes(&client_hello, s->s3->client_random,
		    sizeof(s->s3->client_random)))
			goto err;

		/* Session ID */
		if (!CBB_add_u8_length_prefixed(&client_hello, &session_id))
			goto err;
		if (!s->internal->new_session &&
		    s->session->session_id_length > 0) {
			sl = s->session->session_id_length;
			if (sl > sizeof(s->session->session_id)) {
				SSLerror(s, ERR_R_INTERNAL_ERROR);
				goto err;
			}
			if (!CBB_add_bytes(&session_id,
			    s->session->session_id, sl))
				goto err;
		}

		/* DTLS Cookie. */
		if (SSL_is_dtls(s)) {
			if (s->d1->cookie_len > sizeof(s->d1->cookie)) {
				SSLerror(s, ERR_R_INTERNAL_ERROR);
				goto err;
			}
			if (!CBB_add_u8_length_prefixed(&client_hello, &cookie))
				goto err;
			if (!CBB_add_bytes(&cookie, s->d1->cookie,
			    s->d1->cookie_len))
				goto err;
		}

		/* Ciphers supported */
		if (!CBB_add_u16_length_prefixed(&client_hello, &cipher_suites))
			return 0;
		if (!ssl_cipher_list_to_bytes(s, SSL_get_ciphers(s),
		    &cipher_suites)) {
			SSLerror(s, SSL_R_NO_CIPHERS_AVAILABLE);
			goto err;
		}

		/* Add in compression methods (null) */
		if (!CBB_add_u8_length_prefixed(&client_hello,
		    &compression_methods))
			goto err;
		if (!CBB_add_u8(&compression_methods, 0))
			goto err;

		/* TLS extensions */
		if (!tlsext_client_build(s, SSL_TLSEXT_MSG_CH, &client_hello)) {
			SSLerror(s, ERR_R_INTERNAL_ERROR);
			goto err;
		}

		if (!ssl3_handshake_msg_finish(s, &cbb))
			goto err;

		S3I(s)->hs.state = SSL3_ST_CW_CLNT_HELLO_B;
	}

	/* SSL3_ST_CW_CLNT_HELLO_B */
	return (ssl3_handshake_write(s));

 err:
	CBB_cleanup(&cbb);

	return (-1);
}

int
ssl3_get_dtls_hello_verify(SSL *s)
{
	CBS hello_verify_request, cookie;
	size_t cookie_len;
	uint16_t ssl_version;
	int al, ret;

	if ((ret = ssl3_get_message(s, DTLS1_ST_CR_HELLO_VERIFY_REQUEST_A,
	    DTLS1_ST_CR_HELLO_VERIFY_REQUEST_B, -1, s->internal->max_cert_list)) <= 0)
		return ret;

	if (S3I(s)->hs.tls12.message_type != DTLS1_MT_HELLO_VERIFY_REQUEST) {
		s->d1->send_cookie = 0;
		S3I(s)->hs.tls12.reuse_message = 1;
		return (1);
	}

	if (s->internal->init_num < 0)
		goto decode_err;

	CBS_init(&hello_verify_request, s->internal->init_msg,
	    s->internal->init_num);

	if (!CBS_get_u16(&hello_verify_request, &ssl_version))
		goto decode_err;
	if (!CBS_get_u8_length_prefixed(&hello_verify_request, &cookie))
		goto decode_err;
	if (CBS_len(&hello_verify_request) != 0)
		goto decode_err;

	/*
	 * Per RFC 6347 section 4.2.1, the HelloVerifyRequest should always
	 * contain DTLSv1.0 the version that is going to be negotiated.
	 * Tolerate DTLSv1.2 just in case.
	 */
	if (ssl_version != DTLS1_VERSION && ssl_version != DTLS1_2_VERSION) {
		SSLerror(s, SSL_R_WRONG_SSL_VERSION);
		s->version = (s->version & 0xff00) | (ssl_version & 0xff);
		al = SSL_AD_PROTOCOL_VERSION;
		goto fatal_err;
	}

	if (!CBS_write_bytes(&cookie, s->d1->cookie,
	    sizeof(s->d1->cookie), &cookie_len)) {
		s->d1->cookie_len = 0;
		al = SSL_AD_ILLEGAL_PARAMETER;
		goto fatal_err;
	}
	s->d1->cookie_len = cookie_len;
	s->d1->send_cookie = 1;

	return 1;

 decode_err:
	al = SSL_AD_DECODE_ERROR;
 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, al);
	return -1;
}

int
ssl3_get_server_hello(SSL *s)
{
	CBS cbs, server_random, session_id;
	uint16_t server_version, cipher_suite;
	uint8_t compression_method;
	const SSL_CIPHER *cipher;
	const SSL_METHOD *method;
	unsigned long alg_k;
	size_t outlen;
	int al, ret;

	s->internal->first_packet = 1;
	if ((ret = ssl3_get_message(s, SSL3_ST_CR_SRVR_HELLO_A,
	    SSL3_ST_CR_SRVR_HELLO_B, -1, 20000 /* ?? */)) <= 0)
		return ret;
	s->internal->first_packet = 0;

	if (s->internal->init_num < 0)
		goto decode_err;

	CBS_init(&cbs, s->internal->init_msg, s->internal->init_num);

	if (SSL_is_dtls(s)) {
		if (S3I(s)->hs.tls12.message_type == DTLS1_MT_HELLO_VERIFY_REQUEST) {
			if (s->d1->send_cookie == 0) {
				S3I(s)->hs.tls12.reuse_message = 1;
				return (1);
			} else {
				/* Already sent a cookie. */
				al = SSL_AD_UNEXPECTED_MESSAGE;
				SSLerror(s, SSL_R_BAD_MESSAGE_TYPE);
				goto fatal_err;
			}
		}
	}

	if (S3I(s)->hs.tls12.message_type != SSL3_MT_SERVER_HELLO) {
		al = SSL_AD_UNEXPECTED_MESSAGE;
		SSLerror(s, SSL_R_BAD_MESSAGE_TYPE);
		goto fatal_err;
	}

	if (!CBS_get_u16(&cbs, &server_version))
		goto decode_err;

	if (!ssl_check_version_from_server(s, server_version)) {
		SSLerror(s, SSL_R_WRONG_SSL_VERSION);
		s->version = (s->version & 0xff00) | (server_version & 0xff);
		al = SSL_AD_PROTOCOL_VERSION;
		goto fatal_err;
	}
	S3I(s)->hs.peer_legacy_version = server_version;
	s->version = server_version;

	S3I(s)->hs.negotiated_tls_version = ssl_tls_version(server_version);
	if (S3I(s)->hs.negotiated_tls_version == 0) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}

	if ((method = ssl_get_method(server_version)) == NULL) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}
	s->method = method;

	/* Server random. */
	if (!CBS_get_bytes(&cbs, &server_random, SSL3_RANDOM_SIZE))
		goto decode_err;
	if (!CBS_write_bytes(&server_random, s->s3->server_random,
	    sizeof(s->s3->server_random), NULL))
		goto err;

	if (S3I(s)->hs.our_max_tls_version >= TLS1_2_VERSION &&
	    S3I(s)->hs.negotiated_tls_version < S3I(s)->hs.our_max_tls_version) {
		/*
		 * RFC 8446 section 4.1.3. We must not downgrade if the server
		 * random value contains the TLS 1.2 or TLS 1.1 magical value.
		 */
		if (!CBS_skip(&server_random,
		    CBS_len(&server_random) - sizeof(tls13_downgrade_12)))
			goto err;
		if (S3I(s)->hs.negotiated_tls_version == TLS1_2_VERSION &&
		    CBS_mem_equal(&server_random, tls13_downgrade_12,
		    sizeof(tls13_downgrade_12))) {
			al = SSL_AD_ILLEGAL_PARAMETER;
			SSLerror(s, SSL_R_INAPPROPRIATE_FALLBACK);
			goto fatal_err;
		}
		if (CBS_mem_equal(&server_random, tls13_downgrade_11,
		    sizeof(tls13_downgrade_11))) {
			al = SSL_AD_ILLEGAL_PARAMETER;
			SSLerror(s, SSL_R_INAPPROPRIATE_FALLBACK);
			goto fatal_err;
		}
	}

	/* Session ID. */
	if (!CBS_get_u8_length_prefixed(&cbs, &session_id))
		goto decode_err;

	if (CBS_len(&session_id) > SSL3_SESSION_ID_SIZE) {
		al = SSL_AD_ILLEGAL_PARAMETER;
		SSLerror(s, SSL_R_SSL3_SESSION_ID_TOO_LONG);
		goto fatal_err;
	}

	/* Cipher suite. */
	if (!CBS_get_u16(&cbs, &cipher_suite))
		goto decode_err;

	/*
	 * Check if we want to resume the session based on external
	 * pre-shared secret.
	 */
	if (s->internal->tls_session_secret_cb) {
		SSL_CIPHER *pref_cipher = NULL;
		s->session->master_key_length = sizeof(s->session->master_key);
		if (s->internal->tls_session_secret_cb(s, s->session->master_key,
		    &s->session->master_key_length, NULL, &pref_cipher,
		    s->internal->tls_session_secret_cb_arg)) {
			s->session->cipher = pref_cipher ? pref_cipher :
			    ssl3_get_cipher_by_value(cipher_suite);
			s->s3->flags |= SSL3_FLAGS_CCS_OK;
		}
	}

	if (s->session->session_id_length != 0 &&
	    CBS_mem_equal(&session_id, s->session->session_id,
		s->session->session_id_length)) {
		if (s->sid_ctx_length != s->session->sid_ctx_length ||
		    timingsafe_memcmp(s->session->sid_ctx,
		    s->sid_ctx, s->sid_ctx_length) != 0) {
			/* actually a client application bug */
			al = SSL_AD_ILLEGAL_PARAMETER;
			SSLerror(s, SSL_R_ATTEMPT_TO_REUSE_SESSION_IN_DIFFERENT_CONTEXT);
			goto fatal_err;
		}
		s->s3->flags |= SSL3_FLAGS_CCS_OK;
		s->internal->hit = 1;
	} else {
		/* a miss or crap from the other end */

		/* If we were trying for session-id reuse, make a new
		 * SSL_SESSION so we don't stuff up other people */
		s->internal->hit = 0;
		if (s->session->session_id_length > 0) {
			if (!ssl_get_new_session(s, 0)) {
				al = SSL_AD_INTERNAL_ERROR;
				goto fatal_err;
			}
		}

		/*
		 * XXX - improve the handling for the case where there is a
		 * zero length session identifier.
		 */
		if (!CBS_write_bytes(&session_id, s->session->session_id,
		    sizeof(s->session->session_id), &outlen))
			goto err;
		s->session->session_id_length = outlen;

		s->session->ssl_version = s->version;
	}

	if ((cipher = ssl3_get_cipher_by_value(cipher_suite)) == NULL) {
		al = SSL_AD_ILLEGAL_PARAMETER;
		SSLerror(s, SSL_R_UNKNOWN_CIPHER_RETURNED);
		goto fatal_err;
	}

	/* TLS v1.2 only ciphersuites require v1.2 or later. */
	if ((cipher->algorithm_ssl & SSL_TLSV1_2) &&
	    S3I(s)->hs.negotiated_tls_version < TLS1_2_VERSION) {
		al = SSL_AD_ILLEGAL_PARAMETER;
		SSLerror(s, SSL_R_WRONG_CIPHER_RETURNED);
		goto fatal_err;
	}

	if (!ssl_cipher_in_list(SSL_get_ciphers(s), cipher)) {
		/* we did not say we would use this cipher */
		al = SSL_AD_ILLEGAL_PARAMETER;
		SSLerror(s, SSL_R_WRONG_CIPHER_RETURNED);
		goto fatal_err;
	}

	/*
	 * Depending on the session caching (internal/external), the cipher
	 * and/or cipher_id values may not be set. Make sure that
	 * cipher_id is set and use it for comparison.
	 */
	if (s->session->cipher)
		s->session->cipher_id = s->session->cipher->id;
	if (s->internal->hit && (s->session->cipher_id != cipher->id)) {
		al = SSL_AD_ILLEGAL_PARAMETER;
		SSLerror(s, SSL_R_OLD_SESSION_CIPHER_NOT_RETURNED);
		goto fatal_err;
	}
	S3I(s)->hs.cipher = cipher;

	if (!tls1_transcript_hash_init(s))
		goto err;

	/*
	 * Don't digest cached records if no sigalgs: we may need them for
	 * client authentication.
	 */
	alg_k = S3I(s)->hs.cipher->algorithm_mkey;
	if (!(SSL_USE_SIGALGS(s) || (alg_k & SSL_kGOST)))
		tls1_transcript_free(s);

	if (!CBS_get_u8(&cbs, &compression_method))
		goto decode_err;

	if (compression_method != 0) {
		al = SSL_AD_ILLEGAL_PARAMETER;
		SSLerror(s, SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM);
		goto fatal_err;
	}

	if (!tlsext_client_parse(s, SSL_TLSEXT_MSG_SH, &cbs, &al)) {
		SSLerror(s, SSL_R_PARSE_TLSEXT);
		goto fatal_err;
	}

	if (CBS_len(&cbs) != 0)
		goto decode_err;

	/*
	 * Determine if we need to see RI. Strictly speaking if we want to
	 * avoid an attack we should *always* see RI even on initial server
	 * hello because the client doesn't see any renegotiation during an
	 * attack. However this would mean we could not connect to any server
	 * which doesn't support RI so for the immediate future tolerate RI
	 * absence on initial connect only.
	 */
	if (!S3I(s)->renegotiate_seen &&
	    !(s->internal->options & SSL_OP_LEGACY_SERVER_CONNECT)) {
		al = SSL_AD_HANDSHAKE_FAILURE;
		SSLerror(s, SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED);
		goto fatal_err;
	}

	if (ssl_check_serverhello_tlsext(s) <= 0) {
		SSLerror(s, SSL_R_SERVERHELLO_TLSEXT);
		goto err;
	}

	return (1);

 decode_err:
	/* wrong packet length */
	al = SSL_AD_DECODE_ERROR;
	SSLerror(s, SSL_R_BAD_PACKET_LENGTH);
 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, al);
 err:
	return (-1);
}

int
ssl3_get_server_certificate(SSL *s)
{
	int al, i, ret;
	CBS cbs, cert_list;
	X509 *x = NULL;
	const unsigned char *q;
	STACK_OF(X509) *sk = NULL;
	SESS_CERT *sc;
	EVP_PKEY *pkey = NULL;

	if ((ret = ssl3_get_message(s, SSL3_ST_CR_CERT_A,
	    SSL3_ST_CR_CERT_B, -1, s->internal->max_cert_list)) <= 0)
		return ret;

	ret = -1;

	if (S3I(s)->hs.tls12.message_type == SSL3_MT_SERVER_KEY_EXCHANGE) {
		S3I(s)->hs.tls12.reuse_message = 1;
		return (1);
	}

	if (S3I(s)->hs.tls12.message_type != SSL3_MT_CERTIFICATE) {
		al = SSL_AD_UNEXPECTED_MESSAGE;
		SSLerror(s, SSL_R_BAD_MESSAGE_TYPE);
		goto fatal_err;
	}

	if ((sk = sk_X509_new_null()) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	if (s->internal->init_num < 0)
		goto decode_err;

	CBS_init(&cbs, s->internal->init_msg, s->internal->init_num);
	if (CBS_len(&cbs) < 3)
		goto decode_err;

	if (!CBS_get_u24_length_prefixed(&cbs, &cert_list) ||
	    CBS_len(&cbs) != 0) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto fatal_err;
	}

	while (CBS_len(&cert_list) > 0) {
		CBS cert;

		if (CBS_len(&cert_list) < 3)
			goto decode_err;
		if (!CBS_get_u24_length_prefixed(&cert_list, &cert)) {
			al = SSL_AD_DECODE_ERROR;
			SSLerror(s, SSL_R_CERT_LENGTH_MISMATCH);
			goto fatal_err;
		}

		q = CBS_data(&cert);
		x = d2i_X509(NULL, &q, CBS_len(&cert));
		if (x == NULL) {
			al = SSL_AD_BAD_CERTIFICATE;
			SSLerror(s, ERR_R_ASN1_LIB);
			goto fatal_err;
		}
		if (q != CBS_data(&cert) + CBS_len(&cert)) {
			al = SSL_AD_DECODE_ERROR;
			SSLerror(s, SSL_R_CERT_LENGTH_MISMATCH);
			goto fatal_err;
		}
		if (!sk_X509_push(sk, x)) {
			SSLerror(s, ERR_R_MALLOC_FAILURE);
			goto err;
		}
		x = NULL;
	}

	i = ssl_verify_cert_chain(s, sk);
	if ((s->verify_mode != SSL_VERIFY_NONE) && (i <= 0)) {
		al = ssl_verify_alarm_type(s->verify_result);
		SSLerror(s, SSL_R_CERTIFICATE_VERIFY_FAILED);
		goto fatal_err;

	}
	ERR_clear_error(); /* but we keep s->verify_result */

	sc = ssl_sess_cert_new();
	if (sc == NULL)
		goto err;
	ssl_sess_cert_free(s->session->sess_cert);
	s->session->sess_cert = sc;

	sc->cert_chain = sk;
	/*
	 * Inconsistency alert: cert_chain does include the peer's
	 * certificate, which we don't include in s3_srvr.c
	 */
	x = sk_X509_value(sk, 0);
	sk = NULL;
	/* VRS 19990621: possible memory leak; sk=null ==> !sk_pop_free() @end*/

	pkey = X509_get_pubkey(x);

	if (pkey == NULL || EVP_PKEY_missing_parameters(pkey)) {
		x = NULL;
		al = SSL3_AL_FATAL;
		SSLerror(s, SSL_R_UNABLE_TO_FIND_PUBLIC_KEY_PARAMETERS);
		goto fatal_err;
	}

	i = ssl_cert_type(x, pkey);
	if (i < 0) {
		x = NULL;
		al = SSL3_AL_FATAL;
		SSLerror(s, SSL_R_UNKNOWN_CERTIFICATE_TYPE);
		goto fatal_err;
	}

	sc->peer_cert_type = i;
	X509_up_ref(x);
	/*
	 * Why would the following ever happen?
	 * We just created sc a couple of lines ago.
	 */
	X509_free(sc->peer_pkeys[i].x509);
	sc->peer_pkeys[i].x509 = x;
	sc->peer_key = &(sc->peer_pkeys[i]);

	X509_free(s->session->peer);
	X509_up_ref(x);
	s->session->peer = x;
	s->session->verify_result = s->verify_result;

	x = NULL;
	ret = 1;

	if (0) {
 decode_err:
		/* wrong packet length */
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_BAD_PACKET_LENGTH);
 fatal_err:
		ssl3_send_alert(s, SSL3_AL_FATAL, al);
	}
 err:
	EVP_PKEY_free(pkey);
	X509_free(x);
	sk_X509_pop_free(sk, X509_free);

	return (ret);
}

static int
ssl3_get_server_kex_dhe(SSL *s, EVP_PKEY **pkey, CBS *cbs)
{
	CBS dhp, dhg, dhpk;
	BN_CTX *bn_ctx = NULL;
	SESS_CERT *sc = NULL;
	DH *dh = NULL;
	long alg_a;
	int al;

	alg_a = S3I(s)->hs.cipher->algorithm_auth;
	sc = s->session->sess_cert;

	if ((dh = DH_new()) == NULL) {
		SSLerror(s, ERR_R_DH_LIB);
		goto err;
	}

	if (!CBS_get_u16_length_prefixed(cbs, &dhp))
		goto decode_err;
	if ((dh->p = BN_bin2bn(CBS_data(&dhp), CBS_len(&dhp), NULL)) == NULL) {
		SSLerror(s, ERR_R_BN_LIB);
		goto err;
	}

	if (!CBS_get_u16_length_prefixed(cbs, &dhg))
		goto decode_err;
	if ((dh->g = BN_bin2bn(CBS_data(&dhg), CBS_len(&dhg), NULL)) == NULL) {
		SSLerror(s, ERR_R_BN_LIB);
		goto err;
	}

	if (!CBS_get_u16_length_prefixed(cbs, &dhpk))
		goto decode_err;
	if ((dh->pub_key = BN_bin2bn(CBS_data(&dhpk), CBS_len(&dhpk),
	    NULL)) == NULL) {
		SSLerror(s, ERR_R_BN_LIB);
		goto err;
	}

	/*
	 * Check the strength of the DH key just constructed.
	 * Discard keys weaker than 1024 bits.
	 */
	if (DH_size(dh) < 1024 / 8) {
		SSLerror(s, SSL_R_BAD_DH_P_LENGTH);
		goto err;
	}

	if (alg_a & SSL_aRSA)
		*pkey = X509_get_pubkey(sc->peer_pkeys[SSL_PKEY_RSA].x509);
	else
		/* XXX - Anonymous DH, so no certificate or pkey. */
		*pkey = NULL;

	sc->peer_dh_tmp = dh;

	return (1);

 decode_err:
	al = SSL_AD_DECODE_ERROR;
	SSLerror(s, SSL_R_BAD_PACKET_LENGTH);
	ssl3_send_alert(s, SSL3_AL_FATAL, al);

 err:
	DH_free(dh);
	BN_CTX_free(bn_ctx);

	return (-1);
}

static int
ssl3_get_server_kex_ecdhe_ecp(SSL *s, SESS_CERT *sc, int nid, CBS *public)
{
	EC_KEY *ecdh = NULL;
	int ret = -1;

	/* Extract the server's ephemeral ECDH public key. */
	if ((ecdh = EC_KEY_new()) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if (!ssl_kex_peer_public_ecdhe_ecp(ecdh, nid, public)) {
		SSLerror(s, SSL_R_BAD_ECPOINT);
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
		goto err;
	}

	sc->peer_nid = nid;
	sc->peer_ecdh_tmp = ecdh;
	ecdh = NULL;

	ret = 1;

 err:
	EC_KEY_free(ecdh);

	return (ret);
}

static int
ssl3_get_server_kex_ecdhe_ecx(SSL *s, SESS_CERT *sc, int nid, CBS *public)
{
	size_t outlen;

	if (nid != NID_X25519) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}

	if (CBS_len(public) != X25519_KEY_LENGTH) {
		SSLerror(s, SSL_R_BAD_ECPOINT);
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
		goto err;
	}

	if (!CBS_stow(public, &sc->peer_x25519_tmp, &outlen)) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	return (1);

 err:
	return (-1);
}

static int
ssl3_get_server_kex_ecdhe(SSL *s, EVP_PKEY **pkey, CBS *cbs)
{
	CBS public;
	uint8_t curve_type;
	uint16_t curve_id;
	SESS_CERT *sc;
	long alg_a;
	int nid;
	int al;

	alg_a = S3I(s)->hs.cipher->algorithm_auth;
	sc = s->session->sess_cert;

	/* Only named curves are supported. */
	if (!CBS_get_u8(cbs, &curve_type) ||
	    curve_type != NAMED_CURVE_TYPE ||
	    !CBS_get_u16(cbs, &curve_id)) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_TOO_SHORT);
		goto fatal_err;
	}

	/*
	 * Check that the curve is one of our preferences - if it is not,
	 * the server has sent us an invalid curve.
	 */
	if (tls1_check_curve(s, curve_id) != 1) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_WRONG_CURVE);
		goto fatal_err;
	}

	if ((nid = tls1_ec_curve_id2nid(curve_id)) == 0) {
		al = SSL_AD_INTERNAL_ERROR;
		SSLerror(s, SSL_R_UNABLE_TO_FIND_ECDH_PARAMETERS);
		goto fatal_err;
	}

	if (!CBS_get_u8_length_prefixed(cbs, &public))
		goto decode_err;

	if (nid == NID_X25519) {
		if (ssl3_get_server_kex_ecdhe_ecx(s, sc, nid, &public) != 1)
			goto err;
	} else {
		if (ssl3_get_server_kex_ecdhe_ecp(s, sc, nid, &public) != 1)
			goto err;
	}

	/*
	 * The ECC/TLS specification does not mention the use of DSA to sign
	 * ECParameters in the server key exchange message. We do support RSA
	 * and ECDSA.
	 */
	if (alg_a & SSL_aRSA)
		*pkey = X509_get_pubkey(sc->peer_pkeys[SSL_PKEY_RSA].x509);
	else if (alg_a & SSL_aECDSA)
		*pkey = X509_get_pubkey(sc->peer_pkeys[SSL_PKEY_ECC].x509);
	else
		/* XXX - Anonymous ECDH, so no certificate or pkey. */
		*pkey = NULL;

	return (1);

 decode_err:
	al = SSL_AD_DECODE_ERROR;
	SSLerror(s, SSL_R_BAD_PACKET_LENGTH);

 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, al);

 err:
	return (-1);
}

int
ssl3_get_server_key_exchange(SSL *s)
{
	CBS cbs, signature;
	EVP_PKEY *pkey = NULL;
	EVP_MD_CTX md_ctx;
	const unsigned char *param;
	size_t param_len;
	long alg_k, alg_a;
	int al, ret;

	EVP_MD_CTX_init(&md_ctx);

	alg_k = S3I(s)->hs.cipher->algorithm_mkey;
	alg_a = S3I(s)->hs.cipher->algorithm_auth;

	/*
	 * Use same message size as in ssl3_get_certificate_request()
	 * as ServerKeyExchange message may be skipped.
	 */
	if ((ret = ssl3_get_message(s, SSL3_ST_CR_KEY_EXCH_A,
	    SSL3_ST_CR_KEY_EXCH_B, -1, s->internal->max_cert_list)) <= 0)
		return ret;

	if (s->internal->init_num < 0)
		goto err;

	CBS_init(&cbs, s->internal->init_msg, s->internal->init_num);

	if (S3I(s)->hs.tls12.message_type != SSL3_MT_SERVER_KEY_EXCHANGE) {
		/*
		 * Do not skip server key exchange if this cipher suite uses
		 * ephemeral keys.
		 */
		if (alg_k & (SSL_kDHE|SSL_kECDHE)) {
			SSLerror(s, SSL_R_UNEXPECTED_MESSAGE);
			al = SSL_AD_UNEXPECTED_MESSAGE;
			goto fatal_err;
		}

		S3I(s)->hs.tls12.reuse_message = 1;
		EVP_MD_CTX_cleanup(&md_ctx);
		return (1);
	}

	if (s->session->sess_cert != NULL) {
		DH_free(s->session->sess_cert->peer_dh_tmp);
		s->session->sess_cert->peer_dh_tmp = NULL;

		EC_KEY_free(s->session->sess_cert->peer_ecdh_tmp);
		s->session->sess_cert->peer_ecdh_tmp = NULL;

		free(s->session->sess_cert->peer_x25519_tmp);
		s->session->sess_cert->peer_x25519_tmp = NULL;
	} else {
		s->session->sess_cert = ssl_sess_cert_new();
		if (s->session->sess_cert == NULL)
			goto err;
	}

	param = CBS_data(&cbs);
	param_len = CBS_len(&cbs);

	if (alg_k & SSL_kDHE) {
		if (ssl3_get_server_kex_dhe(s, &pkey, &cbs) != 1)
			goto err;
	} else if (alg_k & SSL_kECDHE) {
		if (ssl3_get_server_kex_ecdhe(s, &pkey, &cbs) != 1)
			goto err;
	} else if (alg_k != 0) {
		al = SSL_AD_UNEXPECTED_MESSAGE;
		SSLerror(s, SSL_R_UNEXPECTED_MESSAGE);
		goto fatal_err;
	}

	param_len -= CBS_len(&cbs);

	/* if it was signed, check the signature */
	if (pkey != NULL) {
		uint16_t sigalg_value = SIGALG_NONE;
		const struct ssl_sigalg *sigalg;
		EVP_PKEY_CTX *pctx;

		if (SSL_USE_SIGALGS(s)) {
			if (!CBS_get_u16(&cbs, &sigalg_value))
				goto decode_err;
		}
		if (!CBS_get_u16_length_prefixed(&cbs, &signature))
			goto decode_err;
		if (CBS_len(&signature) > EVP_PKEY_size(pkey)) {
			al = SSL_AD_DECODE_ERROR;
			SSLerror(s, SSL_R_WRONG_SIGNATURE_LENGTH);
			goto fatal_err;
		}

		if ((sigalg = ssl_sigalg_for_peer(s, pkey,
		    sigalg_value)) == NULL) {
			al = SSL_AD_DECODE_ERROR;
			goto fatal_err;
		}
		S3I(s)->hs.peer_sigalg = sigalg;

		if (!EVP_DigestVerifyInit(&md_ctx, &pctx, sigalg->md(),
		    NULL, pkey))
			goto err;
		if (!EVP_DigestVerifyUpdate(&md_ctx, s->s3->client_random,
		    SSL3_RANDOM_SIZE))
			goto err;
		if ((sigalg->flags & SIGALG_FLAG_RSA_PSS) &&
		    (!EVP_PKEY_CTX_set_rsa_padding(pctx,
		    RSA_PKCS1_PSS_PADDING) ||
		    !EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, -1)))
			goto err;
		if (!EVP_DigestVerifyUpdate(&md_ctx, s->s3->server_random,
		    SSL3_RANDOM_SIZE))
			goto err;
		if (!EVP_DigestVerifyUpdate(&md_ctx, param, param_len))
			goto err;
		if (EVP_DigestVerifyFinal(&md_ctx, CBS_data(&signature),
		    CBS_len(&signature)) <= 0) {
			al = SSL_AD_DECRYPT_ERROR;
			SSLerror(s, SSL_R_BAD_SIGNATURE);
			goto fatal_err;
		}
	} else {
		/* aNULL does not need public keys. */
		if (!(alg_a & SSL_aNULL)) {
			SSLerror(s, ERR_R_INTERNAL_ERROR);
			goto err;
		}
	}

	if (CBS_len(&cbs) != 0) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_EXTRA_DATA_IN_MESSAGE);
		goto fatal_err;
	}

	EVP_PKEY_free(pkey);
	EVP_MD_CTX_cleanup(&md_ctx);

	return (1);

 decode_err:
	al = SSL_AD_DECODE_ERROR;
	SSLerror(s, SSL_R_BAD_PACKET_LENGTH);

 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, al);

 err:
	EVP_PKEY_free(pkey);
	EVP_MD_CTX_cleanup(&md_ctx);

	return (-1);
}

int
ssl3_get_certificate_request(SSL *s)
{
	CBS cert_request, cert_types, rdn_list;
	X509_NAME *xn = NULL;
	const unsigned char *q;
	STACK_OF(X509_NAME) *ca_sk = NULL;
	int ret;

	if ((ret = ssl3_get_message(s, SSL3_ST_CR_CERT_REQ_A,
	    SSL3_ST_CR_CERT_REQ_B, -1, s->internal->max_cert_list)) <= 0)
		return ret;

	ret = 0;

	S3I(s)->hs.tls12.cert_request = 0;

	if (S3I(s)->hs.tls12.message_type == SSL3_MT_SERVER_DONE) {
		S3I(s)->hs.tls12.reuse_message = 1;
		/*
		 * If we get here we don't need any cached handshake records
		 * as we wont be doing client auth.
		 */
		tls1_transcript_free(s);
		return (1);
	}

	if (S3I(s)->hs.tls12.message_type != SSL3_MT_CERTIFICATE_REQUEST) {
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
		SSLerror(s, SSL_R_WRONG_MESSAGE_TYPE);
		goto err;
	}

	/* TLS does not like anon-DH with client cert */
	if (S3I(s)->hs.cipher->algorithm_auth & SSL_aNULL) {
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_UNEXPECTED_MESSAGE);
		SSLerror(s, SSL_R_TLS_CLIENT_CERT_REQ_WITH_ANON_CIPHER);
		goto err;
	}

	if (s->internal->init_num < 0)
		goto decode_err;
	CBS_init(&cert_request, s->internal->init_msg, s->internal->init_num);

	if ((ca_sk = sk_X509_NAME_new(ca_dn_cmp)) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	if (!CBS_get_u8_length_prefixed(&cert_request, &cert_types))
		goto decode_err;

	if (SSL_USE_SIGALGS(s)) {
		CBS sigalgs;

		if (CBS_len(&cert_request) < 2) {
			SSLerror(s, SSL_R_DATA_LENGTH_TOO_LONG);
			goto err;
		}
		if (!CBS_get_u16_length_prefixed(&cert_request, &sigalgs)) {
			ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
			SSLerror(s, SSL_R_DATA_LENGTH_TOO_LONG);
			goto err;
		}
		if (CBS_len(&sigalgs) % 2 != 0 || CBS_len(&sigalgs) > 64) {
			ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
			SSLerror(s, SSL_R_SIGNATURE_ALGORITHMS_ERROR);
			goto err;
		}
		if (!CBS_stow(&sigalgs, &S3I(s)->hs.sigalgs,
		    &S3I(s)->hs.sigalgs_len))
			goto err;
	}

	/* get the CA RDNs */
	if (CBS_len(&cert_request) < 2) {
		SSLerror(s, SSL_R_DATA_LENGTH_TOO_LONG);
		goto err;
	}

	if (!CBS_get_u16_length_prefixed(&cert_request, &rdn_list) ||
	    CBS_len(&cert_request) != 0) {
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto err;
	}

	while (CBS_len(&rdn_list) > 0) {
		CBS rdn;

		if (CBS_len(&rdn_list) < 2) {
			SSLerror(s, SSL_R_DATA_LENGTH_TOO_LONG);
			goto err;
		}

		if (!CBS_get_u16_length_prefixed(&rdn_list, &rdn)) {
			ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
			SSLerror(s, SSL_R_CA_DN_TOO_LONG);
			goto err;
		}

		q = CBS_data(&rdn);
		if ((xn = d2i_X509_NAME(NULL, &q, CBS_len(&rdn))) == NULL) {
			ssl3_send_alert(s, SSL3_AL_FATAL,
			    SSL_AD_DECODE_ERROR);
			SSLerror(s, ERR_R_ASN1_LIB);
			goto err;
		}

		if (q != CBS_data(&rdn) + CBS_len(&rdn)) {
			ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
			SSLerror(s, SSL_R_CA_DN_LENGTH_MISMATCH);
			goto err;
		}
		if (!sk_X509_NAME_push(ca_sk, xn)) {
			SSLerror(s, ERR_R_MALLOC_FAILURE);
			goto err;
		}
		xn = NULL;	/* avoid free in err block */
	}

	/* we should setup a certificate to return.... */
	S3I(s)->hs.tls12.cert_request = 1;
	sk_X509_NAME_pop_free(S3I(s)->hs.tls12.ca_names, X509_NAME_free);
	S3I(s)->hs.tls12.ca_names = ca_sk;
	ca_sk = NULL;

	ret = 1;
	if (0) {
 decode_err:
		SSLerror(s, SSL_R_BAD_PACKET_LENGTH);
	}
 err:
	X509_NAME_free(xn);
	sk_X509_NAME_pop_free(ca_sk, X509_NAME_free);
	return (ret);
}

static int
ca_dn_cmp(const X509_NAME * const *a, const X509_NAME * const *b)
{
	return (X509_NAME_cmp(*a, *b));
}

int
ssl3_get_new_session_ticket(SSL *s)
{
	uint32_t lifetime_hint;
	CBS cbs, session_ticket;
	int al, ret;

	if ((ret = ssl3_get_message(s, SSL3_ST_CR_SESSION_TICKET_A,
	    SSL3_ST_CR_SESSION_TICKET_B, -1, 16384)) <= 0)
		return ret;

	ret = 0;

	if (S3I(s)->hs.tls12.message_type == SSL3_MT_FINISHED) {
		S3I(s)->hs.tls12.reuse_message = 1;
		return (1);
	}
	if (S3I(s)->hs.tls12.message_type != SSL3_MT_NEWSESSION_TICKET) {
		al = SSL_AD_UNEXPECTED_MESSAGE;
		SSLerror(s, SSL_R_BAD_MESSAGE_TYPE);
		goto fatal_err;
	}

	if (s->internal->init_num < 0) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto fatal_err;
	}

	CBS_init(&cbs, s->internal->init_msg, s->internal->init_num);
	if (!CBS_get_u32(&cbs, &lifetime_hint) ||
	    !CBS_get_u16_length_prefixed(&cbs, &session_ticket) ||
	    CBS_len(&cbs) != 0) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto fatal_err;
	}
	s->session->tlsext_tick_lifetime_hint = lifetime_hint;

	if (!CBS_stow(&session_ticket, &s->session->tlsext_tick,
	    &s->session->tlsext_ticklen)) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	/*
	 * There are two ways to detect a resumed ticket sesion.
	 * One is to set an appropriate session ID and then the server
	 * must return a match in ServerHello. This allows the normal
	 * client session ID matching to work and we know much
	 * earlier that the ticket has been accepted.
	 *
	 * The other way is to set zero length session ID when the
	 * ticket is presented and rely on the handshake to determine
	 * session resumption.
	 *
	 * We choose the former approach because this fits in with
	 * assumptions elsewhere in OpenSSL. The session ID is set
	 * to the SHA256 (or SHA1 is SHA256 is disabled) hash of the
	 * ticket.
	 */
	EVP_Digest(CBS_data(&session_ticket), CBS_len(&session_ticket),
	    s->session->session_id, &s->session->session_id_length,
	    EVP_sha256(), NULL);
	ret = 1;
	return (ret);
 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, al);
 err:
	return (-1);
}

int
ssl3_get_cert_status(SSL *s)
{
	CBS cert_status, response;
	uint8_t	status_type;
	int al, ret;

	if ((ret = ssl3_get_message(s, SSL3_ST_CR_CERT_STATUS_A,
	    SSL3_ST_CR_CERT_STATUS_B, -1, 16384)) <= 0)
		return ret;

	if (S3I(s)->hs.tls12.message_type == SSL3_MT_SERVER_KEY_EXCHANGE) {
		/*
		 * Tell the callback the server did not send us an OSCP
		 * response, and has decided to head directly to key exchange.
		 */
		if (s->ctx->internal->tlsext_status_cb) {
			free(s->internal->tlsext_ocsp_resp);
			s->internal->tlsext_ocsp_resp = NULL;
			s->internal->tlsext_ocsp_resp_len = 0;

			ret = s->ctx->internal->tlsext_status_cb(s,
			    s->ctx->internal->tlsext_status_arg);
			if (ret == 0) {
				al = SSL_AD_BAD_CERTIFICATE_STATUS_RESPONSE;
				SSLerror(s, SSL_R_INVALID_STATUS_RESPONSE);
				goto fatal_err;
			}
			if (ret < 0) {
				al = SSL_AD_INTERNAL_ERROR;
				SSLerror(s, ERR_R_MALLOC_FAILURE);
				goto fatal_err;
			}
		}
		S3I(s)->hs.tls12.reuse_message = 1;
		return (1);
	}

	if (S3I(s)->hs.tls12.message_type != SSL3_MT_CERTIFICATE &&
	    S3I(s)->hs.tls12.message_type != SSL3_MT_CERTIFICATE_STATUS) {
		al = SSL_AD_UNEXPECTED_MESSAGE;
		SSLerror(s, SSL_R_BAD_MESSAGE_TYPE);
		goto fatal_err;
	}

	if (s->internal->init_num < 0) {
		/* need at least status type + length */
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto fatal_err;
	}

	CBS_init(&cert_status, s->internal->init_msg, s->internal->init_num);
	if (!CBS_get_u8(&cert_status, &status_type) ||
	    CBS_len(&cert_status) < 3) {
		/* need at least status type + length */
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto fatal_err;
	}

	if (status_type != TLSEXT_STATUSTYPE_ocsp) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_UNSUPPORTED_STATUS_TYPE);
		goto fatal_err;
	}

	if (!CBS_get_u24_length_prefixed(&cert_status, &response) ||
	    CBS_len(&cert_status) != 0) {
		al = SSL_AD_DECODE_ERROR;
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		goto fatal_err;
	}

	if (!CBS_stow(&response, &s->internal->tlsext_ocsp_resp,
	    &s->internal->tlsext_ocsp_resp_len)) {
		al = SSL_AD_INTERNAL_ERROR;
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto fatal_err;
	}

	if (s->ctx->internal->tlsext_status_cb) {
		int ret;
		ret = s->ctx->internal->tlsext_status_cb(s,
		    s->ctx->internal->tlsext_status_arg);
		if (ret == 0) {
			al = SSL_AD_BAD_CERTIFICATE_STATUS_RESPONSE;
			SSLerror(s, SSL_R_INVALID_STATUS_RESPONSE);
			goto fatal_err;
		}
		if (ret < 0) {
			al = SSL_AD_INTERNAL_ERROR;
			SSLerror(s, ERR_R_MALLOC_FAILURE);
			goto fatal_err;
		}
	}
	return (1);
 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, al);
	return (-1);
}

int
ssl3_get_server_done(SSL *s)
{
	int ret;

	if ((ret = ssl3_get_message(s, SSL3_ST_CR_SRVR_DONE_A,
	    SSL3_ST_CR_SRVR_DONE_B, SSL3_MT_SERVER_DONE, 
	    30 /* should be very small, like 0 :-) */)) <= 0)
		return ret;

	if (s->internal->init_num != 0) {
		/* should contain no data */
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_DECODE_ERROR);
		SSLerror(s, SSL_R_LENGTH_MISMATCH);
		return -1;
	}

	return 1;
}

static int
ssl3_send_client_kex_rsa(SSL *s, SESS_CERT *sess_cert, CBB *cbb)
{
	unsigned char pms[SSL_MAX_MASTER_KEY_LENGTH];
	unsigned char *enc_pms = NULL;
	uint16_t max_legacy_version;
	EVP_PKEY *pkey = NULL;
	int ret = -1;
	int enc_len;
	CBB epms;

	/*
	 * RSA-Encrypted Premaster Secret Message - RFC 5246 section 7.4.7.1.
	 */

	pkey = X509_get_pubkey(sess_cert->peer_pkeys[SSL_PKEY_RSA].x509);
	if (pkey == NULL || pkey->type != EVP_PKEY_RSA ||
	    pkey->pkey.rsa == NULL) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}

	/*
	 * Our maximum legacy protocol version - while RFC 5246 section 7.4.7.1
	 * says "The latest (newest) version supported by the client", if we're
	 * doing RSA key exchange then we have to presume that we're talking to
	 * a server that does not understand the supported versions extension
	 * and therefore our maximum version is that sent in the ClientHello.
	 */
	if (!ssl_max_legacy_version(s, &max_legacy_version))
		goto err;
	pms[0] = max_legacy_version >> 8;
	pms[1] = max_legacy_version & 0xff;
	arc4random_buf(&pms[2], sizeof(pms) - 2);

	if ((enc_pms = malloc(RSA_size(pkey->pkey.rsa))) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	enc_len = RSA_public_encrypt(sizeof(pms), pms, enc_pms, pkey->pkey.rsa,
	    RSA_PKCS1_PADDING);
	if (enc_len <= 0) {
		SSLerror(s, SSL_R_BAD_RSA_ENCRYPT);
		goto err;
	}

	if (!CBB_add_u16_length_prefixed(cbb, &epms))
		goto err;
	if (!CBB_add_bytes(&epms, enc_pms, enc_len))
		goto err;
	if (!CBB_flush(cbb))
		goto err;

	if (!tls12_derive_master_secret(s, pms, sizeof(pms)))
		goto err;

	ret = 1;

 err:
	explicit_bzero(pms, sizeof(pms));
	EVP_PKEY_free(pkey);
	free(enc_pms);

	return (ret);
}

static int
ssl3_send_client_kex_dhe(SSL *s, SESS_CERT *sess_cert, CBB *cbb)
{
	DH *dh_srvr = NULL, *dh_clnt = NULL;
	unsigned char *key = NULL;
	int key_size = 0, key_len;
	unsigned char *data;
	int ret = -1;
	CBB dh_Yc;

	/* Ensure that we have an ephemeral key for DHE. */
	if (sess_cert->peer_dh_tmp == NULL) {
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
		SSLerror(s, SSL_R_UNABLE_TO_FIND_DH_PARAMETERS);
		goto err;
	}
	dh_srvr = sess_cert->peer_dh_tmp;

	/* Generate a new random key. */
	if ((dh_clnt = DHparams_dup(dh_srvr)) == NULL) {
		SSLerror(s, ERR_R_DH_LIB);
		goto err;
	}
	if (!DH_generate_key(dh_clnt)) {
		SSLerror(s, ERR_R_DH_LIB);
		goto err;
	}
	if ((key_size = DH_size(dh_clnt)) <= 0) {
		SSLerror(s, ERR_R_DH_LIB);
		goto err;
	}
	if ((key = malloc(key_size)) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if ((key_len = DH_compute_key(key, dh_srvr->pub_key, dh_clnt)) <= 0) {
		SSLerror(s, ERR_R_DH_LIB);
		goto err;
	}

	if (!tls12_derive_master_secret(s, key, key_len))
		goto err;

	if (!CBB_add_u16_length_prefixed(cbb, &dh_Yc))
		goto err;
	if (!CBB_add_space(&dh_Yc, &data, BN_num_bytes(dh_clnt->pub_key)))
		goto err;
	BN_bn2bin(dh_clnt->pub_key, data);
	if (!CBB_flush(cbb))
		goto err;

	ret = 1;

 err:
	DH_free(dh_clnt);
	freezero(key, key_size);

	return (ret);
}

static int
ssl3_send_client_kex_ecdhe_ecp(SSL *s, SESS_CERT *sc, CBB *cbb)
{
	EC_KEY *ecdh = NULL;
	uint8_t *key = NULL;
	size_t key_len = 0;
	int ret = -1;
	CBB ecpoint;

	if ((ecdh = EC_KEY_new()) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	if (!ssl_kex_generate_ecdhe_ecp(ecdh, sc->peer_nid))
		goto err;

	/* Encode our public key. */
	if (!CBB_add_u8_length_prefixed(cbb, &ecpoint))
		goto err;
	if (!ssl_kex_public_ecdhe_ecp(ecdh, &ecpoint))
		goto err;
	if (!CBB_flush(cbb))
		goto err;

	if (!ssl_kex_derive_ecdhe_ecp(ecdh, sc->peer_ecdh_tmp, &key, &key_len))
		goto err;
	if (!tls12_derive_master_secret(s, key, key_len))
		goto err;

	ret = 1;

 err:
	freezero(key, key_len);
	EC_KEY_free(ecdh);

	return (ret);
}

static int
ssl3_send_client_kex_ecdhe_ecx(SSL *s, SESS_CERT *sc, CBB *cbb)
{
	uint8_t *public_key = NULL, *private_key = NULL, *shared_key = NULL;
	int ret = -1;
	CBB ecpoint;

	/* Generate X25519 key pair and derive shared key. */
	if ((public_key = malloc(X25519_KEY_LENGTH)) == NULL)
		goto err;
	if ((private_key = malloc(X25519_KEY_LENGTH)) == NULL)
		goto err;
	if ((shared_key = malloc(X25519_KEY_LENGTH)) == NULL)
		goto err;
	X25519_keypair(public_key, private_key);
	if (!X25519(shared_key, private_key, sc->peer_x25519_tmp))
		goto err;

	/* Serialize the public key. */
	if (!CBB_add_u8_length_prefixed(cbb, &ecpoint))
		goto err;
	if (!CBB_add_bytes(&ecpoint, public_key, X25519_KEY_LENGTH))
		goto err;
	if (!CBB_flush(cbb))
		goto err;

	if (!tls12_derive_master_secret(s, shared_key, X25519_KEY_LENGTH))
		goto err;

	ret = 1;

 err:
	free(public_key);
	freezero(private_key, X25519_KEY_LENGTH);
	freezero(shared_key, X25519_KEY_LENGTH);

	return (ret);
}

static int
ssl3_send_client_kex_ecdhe(SSL *s, SESS_CERT *sc, CBB *cbb)
{
	if (sc->peer_x25519_tmp != NULL) {
		if (ssl3_send_client_kex_ecdhe_ecx(s, sc, cbb) != 1)
			goto err;
	} else if (sc->peer_ecdh_tmp != NULL) {
		if (ssl3_send_client_kex_ecdhe_ecp(s, sc, cbb) != 1)
			goto err;
	} else {
		ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}

	return (1);

 err:
	return (-1);
}

static int
ssl3_send_client_kex_gost(SSL *s, SESS_CERT *sess_cert, CBB *cbb)
{
	unsigned char premaster_secret[32], shared_ukm[32], tmp[256];
	EVP_PKEY *pub_key = NULL;
	EVP_PKEY_CTX *pkey_ctx;
	X509 *peer_cert;
	size_t msglen;
	unsigned int md_len;
	EVP_MD_CTX *ukm_hash;
	int ret = -1;
	int nid;
	CBB gostblob;

	/* Get server sertificate PKEY and create ctx from it */
	peer_cert = sess_cert->peer_pkeys[SSL_PKEY_GOST01].x509;
	if (peer_cert == NULL) {
		SSLerror(s, SSL_R_NO_GOST_CERTIFICATE_SENT_BY_PEER);
		goto err;
	}

	pub_key = X509_get_pubkey(peer_cert);
	pkey_ctx = EVP_PKEY_CTX_new(pub_key, NULL);

	/*
	 * If we have send a certificate, and certificate key parameters match
	 * those of server certificate, use certificate key for key exchange.
	 * Otherwise, generate ephemeral key pair.
	 */
	EVP_PKEY_encrypt_init(pkey_ctx);

	/* Generate session key. */
	arc4random_buf(premaster_secret, 32);

	/*
	 * If we have client certificate, use its secret as peer key.
	 */
	if (S3I(s)->hs.tls12.cert_request && s->cert->key->privatekey) {
		if (EVP_PKEY_derive_set_peer(pkey_ctx,
		    s->cert->key->privatekey) <=0) {
			/*
			 * If there was an error - just ignore it.
			 * Ephemeral key would be used.
			 */
			ERR_clear_error();
		}
	}

	/*
	 * Compute shared IV and store it in algorithm-specific context data.
	 */
	ukm_hash = EVP_MD_CTX_new();
	if (ukm_hash == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}

	/* XXX check handshake hash instead. */
	if (S3I(s)->hs.cipher->algorithm2 & SSL_HANDSHAKE_MAC_GOST94)
		nid = NID_id_GostR3411_94;
	else
		nid = NID_id_tc26_gost3411_2012_256;
	if (!EVP_DigestInit(ukm_hash, EVP_get_digestbynid(nid)))
		goto err;
	EVP_DigestUpdate(ukm_hash, s->s3->client_random, SSL3_RANDOM_SIZE);
	EVP_DigestUpdate(ukm_hash, s->s3->server_random, SSL3_RANDOM_SIZE);
	EVP_DigestFinal_ex(ukm_hash, shared_ukm, &md_len);
	EVP_MD_CTX_free(ukm_hash);
	if (EVP_PKEY_CTX_ctrl(pkey_ctx, -1, EVP_PKEY_OP_ENCRYPT,
	    EVP_PKEY_CTRL_SET_IV, 8, shared_ukm) < 0) {
		SSLerror(s, SSL_R_LIBRARY_BUG);
		goto err;
	}

	/*
	 * Make GOST keytransport blob message, encapsulate it into sequence.
	 */
	msglen = 255;
	if (EVP_PKEY_encrypt(pkey_ctx, tmp, &msglen, premaster_secret,
	    32) < 0) {
		SSLerror(s, SSL_R_LIBRARY_BUG);
		goto err;
	}

	if (!CBB_add_asn1(cbb, &gostblob, CBS_ASN1_SEQUENCE))
		goto err;
	if (!CBB_add_bytes(&gostblob, tmp, msglen))
		goto err;
	if (!CBB_flush(cbb))
		goto err;

	/* Check if pubkey from client certificate was used. */
	if (EVP_PKEY_CTX_ctrl(pkey_ctx, -1, -1, EVP_PKEY_CTRL_PEER_KEY, 2,
	    NULL) > 0) {
		/* Set flag "skip certificate verify". */
		s->s3->flags |= TLS1_FLAGS_SKIP_CERT_VERIFY;
	}
	EVP_PKEY_CTX_free(pkey_ctx);

	if (!tls12_derive_master_secret(s, premaster_secret, 32))
		goto err;

	ret = 1;

 err:
	explicit_bzero(premaster_secret, sizeof(premaster_secret));
	EVP_PKEY_free(pub_key);

	return (ret);
}

int
ssl3_send_client_key_exchange(SSL *s)
{
	SESS_CERT *sess_cert;
	unsigned long alg_k;
	CBB cbb, kex;

	memset(&cbb, 0, sizeof(cbb));

	if (S3I(s)->hs.state == SSL3_ST_CW_KEY_EXCH_A) {
		alg_k = S3I(s)->hs.cipher->algorithm_mkey;

		if ((sess_cert = s->session->sess_cert) == NULL) {
			ssl3_send_alert(s, SSL3_AL_FATAL,
			    SSL_AD_UNEXPECTED_MESSAGE);
			SSLerror(s, ERR_R_INTERNAL_ERROR);
			goto err;
		}

		if (!ssl3_handshake_msg_start(s, &cbb, &kex,
		    SSL3_MT_CLIENT_KEY_EXCHANGE))
			goto err;

		if (alg_k & SSL_kRSA) {
			if (ssl3_send_client_kex_rsa(s, sess_cert, &kex) != 1)
				goto err;
		} else if (alg_k & SSL_kDHE) {
			if (ssl3_send_client_kex_dhe(s, sess_cert, &kex) != 1)
				goto err;
		} else if (alg_k & SSL_kECDHE) {
			if (ssl3_send_client_kex_ecdhe(s, sess_cert, &kex) != 1)
				goto err;
		} else if (alg_k & SSL_kGOST) {
			if (ssl3_send_client_kex_gost(s, sess_cert, &kex) != 1)
				goto err;
		} else {
			ssl3_send_alert(s, SSL3_AL_FATAL,
			    SSL_AD_HANDSHAKE_FAILURE);
			SSLerror(s, ERR_R_INTERNAL_ERROR);
			goto err;
		}

		if (!ssl3_handshake_msg_finish(s, &cbb))
			goto err;

		S3I(s)->hs.state = SSL3_ST_CW_KEY_EXCH_B;
	}

	/* SSL3_ST_CW_KEY_EXCH_B */
	return (ssl3_handshake_write(s));

 err:
	CBB_cleanup(&cbb);

	return (-1);
}

static int
ssl3_send_client_verify_sigalgs(SSL *s, EVP_PKEY *pkey,
    const struct ssl_sigalg *sigalg, CBB *cert_verify)
{
	CBB cbb_signature;
	EVP_PKEY_CTX *pctx = NULL;
	EVP_MD_CTX mctx;
	const unsigned char *hdata;
	unsigned char *signature = NULL;
	size_t signature_len, hdata_len;
	int ret = 0;

	EVP_MD_CTX_init(&mctx);

	if (!tls1_transcript_data(s, &hdata, &hdata_len)) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}
	if (!EVP_DigestSignInit(&mctx, &pctx, sigalg->md(), NULL, pkey)) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (sigalg->key_type == EVP_PKEY_GOSTR01 &&
	    EVP_PKEY_CTX_ctrl(pctx, -1, EVP_PKEY_OP_SIGN,
	    EVP_PKEY_CTRL_GOST_SIG_FORMAT, GOST_SIG_FORMAT_RS_LE, NULL) <= 0) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if ((sigalg->flags & SIGALG_FLAG_RSA_PSS) &&
	    (!EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING) ||
	    !EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, -1))) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (!EVP_DigestSignUpdate(&mctx, hdata, hdata_len)) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (!EVP_DigestSignFinal(&mctx, NULL, &signature_len) ||
	    signature_len == 0) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if ((signature = calloc(1, signature_len)) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if (!EVP_DigestSignFinal(&mctx, signature, &signature_len)) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}

	if (!CBB_add_u16(cert_verify, sigalg->value))
		goto err;
	if (!CBB_add_u16_length_prefixed(cert_verify, &cbb_signature))
		goto err;
	if (!CBB_add_bytes(&cbb_signature, signature, signature_len))
		goto err;
	if (!CBB_flush(cert_verify))
		goto err;

	ret = 1;

 err:
	EVP_MD_CTX_cleanup(&mctx);
	free(signature);
	return ret;
}

static int
ssl3_send_client_verify_rsa(SSL *s, EVP_PKEY *pkey, CBB *cert_verify)
{
	CBB cbb_signature;
	unsigned char data[EVP_MAX_MD_SIZE];
	unsigned char *signature = NULL;
	unsigned int signature_len;
	size_t data_len;
	int ret = 0;

	if (!tls1_transcript_hash_value(s, data, sizeof(data), &data_len))
		goto err;
	if ((signature = calloc(1, EVP_PKEY_size(pkey))) == NULL)
		goto err;
	if (RSA_sign(NID_md5_sha1, data, data_len, signature,
	    &signature_len, pkey->pkey.rsa) <= 0 ) {
		SSLerror(s, ERR_R_RSA_LIB);
		goto err;
	}

	if (!CBB_add_u16_length_prefixed(cert_verify, &cbb_signature))
		goto err;
	if (!CBB_add_bytes(&cbb_signature, signature, signature_len))
		goto err;
	if (!CBB_flush(cert_verify))
		goto err;

	ret = 1;
 err:
	free(signature);
	return ret;
}

static int
ssl3_send_client_verify_ec(SSL *s, EVP_PKEY *pkey, CBB *cert_verify)
{
	CBB cbb_signature;
	unsigned char data[EVP_MAX_MD_SIZE];
	unsigned char *signature = NULL;
	unsigned int signature_len;
	int ret = 0;

	if (!tls1_transcript_hash_value(s, data, sizeof(data), NULL))
		goto err;
	if ((signature = calloc(1, EVP_PKEY_size(pkey))) == NULL)
		goto err;
	if (!ECDSA_sign(pkey->save_type, &data[MD5_DIGEST_LENGTH],
	    SHA_DIGEST_LENGTH, signature, &signature_len, pkey->pkey.ec)) {
		SSLerror(s, ERR_R_ECDSA_LIB);
		goto err;
	}

	if (!CBB_add_u16_length_prefixed(cert_verify, &cbb_signature))
		goto err;
	if (!CBB_add_bytes(&cbb_signature, signature, signature_len))
		goto err;
	if (!CBB_flush(cert_verify))
		goto err;

	ret = 1;
 err:
	free(signature);
	return ret;
}

#ifndef OPENSSL_NO_GOST
static int
ssl3_send_client_verify_gost(SSL *s, EVP_PKEY *pkey, CBB *cert_verify)
{
	CBB cbb_signature;
	EVP_MD_CTX mctx;
	EVP_PKEY_CTX *pctx;
	const EVP_MD *md;
	const unsigned char *hdata;
	unsigned char *signature = NULL;
	size_t signature_len;
	size_t hdata_len;
	int nid;
	int ret = 0;

	EVP_MD_CTX_init(&mctx);

	if (!tls1_transcript_data(s, &hdata, &hdata_len)) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}
	if (!EVP_PKEY_get_default_digest_nid(pkey, &nid) ||
	    (md = EVP_get_digestbynid(nid)) == NULL) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (!EVP_DigestSignInit(&mctx, &pctx, md, NULL, pkey)) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (EVP_PKEY_CTX_ctrl(pctx, -1, EVP_PKEY_OP_SIGN,
	    EVP_PKEY_CTRL_GOST_SIG_FORMAT, GOST_SIG_FORMAT_RS_LE, NULL) <= 0) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (!EVP_DigestSignUpdate(&mctx, hdata, hdata_len)) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if (!EVP_DigestSignFinal(&mctx, NULL, &signature_len) ||
	    signature_len == 0) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}
	if ((signature = calloc(1, signature_len)) == NULL) {
		SSLerror(s, ERR_R_MALLOC_FAILURE);
		goto err;
	}
	if (!EVP_DigestSignFinal(&mctx, signature, &signature_len)) {
		SSLerror(s, ERR_R_EVP_LIB);
		goto err;
	}

	if (!CBB_add_u16_length_prefixed(cert_verify, &cbb_signature))
		goto err;
	if (!CBB_add_bytes(&cbb_signature, signature, signature_len))
		goto err;
	if (!CBB_flush(cert_verify))
		goto err;

	ret = 1;
 err:
	EVP_MD_CTX_cleanup(&mctx);
	free(signature);
	return ret;
}
#endif

int
ssl3_send_client_verify(SSL *s)
{
	const struct ssl_sigalg *sigalg;
	CBB cbb, cert_verify;
	EVP_PKEY *pkey;

	memset(&cbb, 0, sizeof(cbb));

	if (S3I(s)->hs.state == SSL3_ST_CW_CERT_VRFY_A) {
		if (!ssl3_handshake_msg_start(s, &cbb, &cert_verify,
		    SSL3_MT_CERTIFICATE_VERIFY))
			goto err;

		pkey = s->cert->key->privatekey;
		if ((sigalg = ssl_sigalg_select(s, pkey)) == NULL) {
			SSLerror(s, SSL_R_SIGNATURE_ALGORITHMS_ERROR);
			goto err;
		}
		S3I(s)->hs.our_sigalg = sigalg;

		/*
		 * For TLS v1.2 send signature algorithm and signature using
		 * agreed digest and cached handshake records.
		 */
		if (SSL_USE_SIGALGS(s)) {
			if (!ssl3_send_client_verify_sigalgs(s, pkey, sigalg,
			    &cert_verify))
				goto err;
		} else if (pkey->type == EVP_PKEY_RSA) {
			if (!ssl3_send_client_verify_rsa(s, pkey, &cert_verify))
				goto err;
		} else if (pkey->type == EVP_PKEY_EC) {
			if (!ssl3_send_client_verify_ec(s, pkey, &cert_verify))
				goto err;
#ifndef OPENSSL_NO_GOST
		} else if (pkey->type == NID_id_GostR3410_94 ||
		    pkey->type == NID_id_GostR3410_2001) {
			if (!ssl3_send_client_verify_gost(s, pkey, &cert_verify))
				goto err;
#endif
		} else {
			SSLerror(s, ERR_R_INTERNAL_ERROR);
			goto err;
		}

		tls1_transcript_free(s);

		if (!ssl3_handshake_msg_finish(s, &cbb))
			goto err;

		S3I(s)->hs.state = SSL3_ST_CW_CERT_VRFY_B;
	}

	return (ssl3_handshake_write(s));

 err:
	CBB_cleanup(&cbb);

	return (-1);
}

int
ssl3_send_client_certificate(SSL *s)
{
	EVP_PKEY *pkey = NULL;
	X509 *x509 = NULL;
	CBB cbb, client_cert;
	int i;

	memset(&cbb, 0, sizeof(cbb));

	if (S3I(s)->hs.state == SSL3_ST_CW_CERT_A) {
		if (s->cert->key->x509 == NULL ||
		    s->cert->key->privatekey == NULL)
			S3I(s)->hs.state = SSL3_ST_CW_CERT_B;
		else
			S3I(s)->hs.state = SSL3_ST_CW_CERT_C;
	}

	/* We need to get a client cert */
	if (S3I(s)->hs.state == SSL3_ST_CW_CERT_B) {
		/*
		 * If we get an error, we need to
		 * ssl->internal->rwstate = SSL_X509_LOOKUP; return(-1);
		 * We then get retried later.
		 */
		i = ssl_do_client_cert_cb(s, &x509, &pkey);
		if (i < 0) {
			s->internal->rwstate = SSL_X509_LOOKUP;
			return (-1);
		}
		s->internal->rwstate = SSL_NOTHING;
		if ((i == 1) && (pkey != NULL) && (x509 != NULL)) {
			S3I(s)->hs.state = SSL3_ST_CW_CERT_B;
			if (!SSL_use_certificate(s, x509) ||
			    !SSL_use_PrivateKey(s, pkey))
				i = 0;
		} else if (i == 1) {
			i = 0;
			SSLerror(s, SSL_R_BAD_DATA_RETURNED_BY_CALLBACK);
		}

		X509_free(x509);
		EVP_PKEY_free(pkey);
		if (i == 0) {
			S3I(s)->hs.tls12.cert_request = 2;

			/* There is no client certificate to verify. */
			tls1_transcript_free(s);
		}

		/* Ok, we have a cert */
		S3I(s)->hs.state = SSL3_ST_CW_CERT_C;
	}

	if (S3I(s)->hs.state == SSL3_ST_CW_CERT_C) {
		if (!ssl3_handshake_msg_start(s, &cbb, &client_cert,
		    SSL3_MT_CERTIFICATE))
			goto err;
		if (!ssl3_output_cert_chain(s, &client_cert,
		    (S3I(s)->hs.tls12.cert_request == 2) ? NULL : s->cert->key))
			goto err;
		if (!ssl3_handshake_msg_finish(s, &cbb))
			goto err;

		S3I(s)->hs.state = SSL3_ST_CW_CERT_D;
	}

	/* SSL3_ST_CW_CERT_D */
	return (ssl3_handshake_write(s));

 err:
	CBB_cleanup(&cbb);

	return (0);
}

#define has_bits(i,m)	(((i)&(m)) == (m))

int
ssl3_check_cert_and_algorithm(SSL *s)
{
	int		 i, idx;
	long		 alg_k, alg_a;
	EVP_PKEY	*pkey = NULL;
	SESS_CERT	*sc;
	DH		*dh;

	alg_k = S3I(s)->hs.cipher->algorithm_mkey;
	alg_a = S3I(s)->hs.cipher->algorithm_auth;

	/* We don't have a certificate. */
	if (alg_a & SSL_aNULL)
		return (1);

	sc = s->session->sess_cert;
	if (sc == NULL) {
		SSLerror(s, ERR_R_INTERNAL_ERROR);
		goto err;
	}
	dh = s->session->sess_cert->peer_dh_tmp;

	/* This is the passed certificate. */

	idx = sc->peer_cert_type;
	if (idx == SSL_PKEY_ECC) {
		if (ssl_check_srvr_ecc_cert_and_alg(
		    sc->peer_pkeys[idx].x509, s) == 0) {
			/* check failed */
			SSLerror(s, SSL_R_BAD_ECC_CERT);
			goto fatal_err;
		} else {
			return (1);
		}
	}
	pkey = X509_get_pubkey(sc->peer_pkeys[idx].x509);
	i = X509_certificate_type(sc->peer_pkeys[idx].x509, pkey);
	EVP_PKEY_free(pkey);

	/* Check that we have a certificate if we require one. */
	if ((alg_a & SSL_aRSA) && !has_bits(i, EVP_PK_RSA|EVP_PKT_SIGN)) {
		SSLerror(s, SSL_R_MISSING_RSA_SIGNING_CERT);
		goto fatal_err;
	}
	if ((alg_k & SSL_kRSA) && !has_bits(i, EVP_PK_RSA|EVP_PKT_ENC)) {
		SSLerror(s, SSL_R_MISSING_RSA_ENCRYPTING_CERT);
		goto fatal_err;
	}
	if ((alg_k & SSL_kDHE) &&
	    !(has_bits(i, EVP_PK_DH|EVP_PKT_EXCH) || (dh != NULL))) {
		SSLerror(s, SSL_R_MISSING_DH_KEY);
		goto fatal_err;
	}

	return (1);
 fatal_err:
	ssl3_send_alert(s, SSL3_AL_FATAL, SSL_AD_HANDSHAKE_FAILURE);
 err:
	return (0);
}

/*
 * Check to see if handshake is full or resumed. Usually this is just a
 * case of checking to see if a cache hit has occurred. In the case of
 * session tickets we have to check the next message to be sure.
 */

int
ssl3_check_finished(SSL *s)
{
	int ret;

	/* If we have no ticket it cannot be a resumed session. */
	if (!s->session->tlsext_tick)
		return (1);
	/* this function is called when we really expect a Certificate
	 * message, so permit appropriate message length */
	if ((ret = ssl3_get_message(s, SSL3_ST_CR_CERT_A,
	    SSL3_ST_CR_CERT_B, -1, s->internal->max_cert_list)) <= 0)
		return ret;

	S3I(s)->hs.tls12.reuse_message = 1;
	if ((S3I(s)->hs.tls12.message_type == SSL3_MT_FINISHED) ||
	    (S3I(s)->hs.tls12.message_type == SSL3_MT_NEWSESSION_TICKET))
		return (2);

	return (1);
}

int
ssl_do_client_cert_cb(SSL *s, X509 **px509, EVP_PKEY **ppkey)
{
	int	i = 0;

#ifndef OPENSSL_NO_ENGINE
	if (s->ctx->internal->client_cert_engine) {
		i = ENGINE_load_ssl_client_cert(
		    s->ctx->internal->client_cert_engine, s,
		    SSL_get_client_CA_list(s), px509, ppkey, NULL, NULL, NULL);
		if (i != 0)
			return (i);
	}
#endif
	if (s->ctx->internal->client_cert_cb)
		i = s->ctx->internal->client_cert_cb(s, px509, ppkey);
	return (i);
}
