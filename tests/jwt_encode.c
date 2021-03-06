/* Public domain, no copyright. Use at your own risk. */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <check.h>

#include <jwt.h>

/* Constant time to make tests consistent. */
#define TS_CONST 1475980545L

/* Macro to allocate a new JWT with checks. */
#define ALLOC_JWT(__jwt) \
	do { \
		int __ret = jwt_new(__jwt); \
		ck_assert_int_eq(__ret, 0); \
		ck_assert_ptr_ne(__jwt, NULL); \
	} while (0)

/* Older check doesn't have this. */
#ifndef ck_assert_ptr_ne
	#define ck_assert_ptr_ne(X, Y) ck_assert(X != Y)
#endif

START_TEST(test_jwt_encode_fp)
{
	FILE * out;
	jwt_t *jwt = NULL;
	int    ret = 0;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	/* TODO Write to actual file and read back to validate output. */
#ifdef _WIN32
	out = fopen("nul", "w");
	ck_assert_ptr_ne(out, NULL);
#else
	out = fopen("/dev/null", "w");
	ck_assert_ptr_ne(out, NULL);
#endif

	ret = jwt_encode_fp(jwt, out);
	ck_assert_int_eq(ret, 0);

	fclose(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_str)
{
	const char res[] =
	    "eyJhbGciOiJub25lIn0.eyJpYXQiOjE0NzU5ODA1NDUsIm"
	    "lzcyI6ImZpbGVzLmN5cGhyZS5jb20iLCJyZWYiOiJYWFhYLVlZWVktWlpa"
	    "Wi1BQUFBLUNDQ0MiLCJzdWIiOiJ1c2VyMCJ9.";
	jwt_t *jwt = NULL;
	int    ret = 0;
	char * out;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	out = jwt_encode_str(jwt);
	ck_assert_ptr_ne(out, NULL);

	ck_assert_str_eq(out, res);

	jwt_free_str(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_alg_none)
{
	const char res[] =
	    "eyJhbGciOiJub25lIn0.eyJhdWQiOiJ3d3cucGx1Z2dlcnM"
	    "ubmwiLCJleHAiOjE0Nzc1MTQ4MTIsInN1YiI6IlBsdWdnZXJzIFNvZnR3YXJlIn0.";
	jwt_t *jwt = NULL;
	int    ret = 0;
	char * out;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "aud", "www.pluggers.nl");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "exp", 1477514812);
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "Pluggers Software");
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_NONE, NULL, 0);
	ck_assert_int_eq(ret, 0);

	out = jwt_encode_str(jwt);
	ck_assert_ptr_ne(out, NULL);

	ck_assert_str_eq(out, res);

	jwt_free_str(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_hs256)
{
	const char res[] =
	    "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE"
	    "0NzU5ODA1NDUsImlzcyI6ImZpbGVzLmN5cGhyZS5jb20iLCJyZWYiOiJYWF"
	    "hYLVlZWVktWlpaWi1BQUFBLUNDQ0MiLCJzdWIiOiJ1c2VyMCJ9.B0a9gqWg"
	    "PuuIx-EFXXSHQByCMHCzs0gjvY3-60oV4TY";
	unsigned char key256[32] = "012345678901234567890123456789XY";
	jwt_t *       jwt        = NULL;
	int           ret        = 0;
	char *        out;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_HS256, key256, sizeof(key256));
	ck_assert_int_eq(ret, 0);

	out = jwt_encode_str(jwt);
	ck_assert_ptr_ne(out, NULL);

	ck_assert_str_eq(out, res);

	jwt_free_str(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_hs384)
{
	const char res[] =
	    "eyJhbGciOiJIUzM4NCIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE"
	    "0NzU5ODA1NDUsImlzcyI6ImZpbGVzLmN5cGhyZS5jb20iLCJyZWYiOiJYWF"
	    "hYLVlZWVktWlpaWi1BQUFBLUNDQ0MiLCJzdWIiOiJ1c2VyMCJ9.k5mpjWlu"
	    "aj4EQxuvoyXHR9HVw_V4GMnguwcQvZplTDT_H2PS0DDoZ5NF-VLC8kgO";
	unsigned char key384[48] = "aaaabbbbccccddddeeeeffffgggghhhh"
	                           "iiiijjjjkkkkllll";
	jwt_t *jwt = NULL;
	int    ret = 0;
	char * out;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_HS384, key384, sizeof(key384));
	ck_assert_int_eq(ret, 0);

	out = jwt_encode_str(jwt);
	ck_assert_ptr_ne(out, NULL);

	ck_assert_str_eq(out, res);

	jwt_free_str(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_hs512)
{
	const char res[] =
	    "eyJhbGciOiJIUzUxMiIsInR5cCI6IkpXVCJ9.eyJpYXQiOjE"
	    "0NzU5ODA1NDUsImlzcyI6ImZpbGVzLmN5cGhyZS5jb20iLCJyZWYiOiJYWF"
	    "hYLVlZWVktWlpaWi1BQUFBLUNDQ0MiLCJzdWIiOiJ1c2VyMCJ9.oxB_kx_h"
	    "5DANiG5oZWPO90MFlkoMb7VGlEBDbBTpX_JThJ8md6UEsxFvwm2weeyHU4-"
	    "MasEU4nzbVk4LZ0vrcg";
	unsigned char key512[64] = "012345678901234567890123456789XY"
	                           "012345678901234567890123456789XY";
	jwt_t *jwt = NULL;
	int    ret = 0;
	char * out;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_HS512, key512, sizeof(key512));
	ck_assert_int_eq(ret, 0);

	out = jwt_encode_str(jwt);
	ck_assert_ptr_ne(out, NULL);

	ck_assert_str_eq(out, res);

	jwt_free_str(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_change_alg)
{
	const char res[] =
	    "eyJhbGciOiJub25lIn0.eyJpYXQiOjE0NzU5ODA1NDUsIm"
	    "lzcyI6ImZpbGVzLmN5cGhyZS5jb20iLCJyZWYiOiJYWFhYLVlZWVktWlpa"
	    "Wi1BQUFBLUNDQ0MiLCJzdWIiOiJ1c2VyMCJ9.";
	unsigned char key512[64] = "012345678901234567890123456789XY"
	                           "012345678901234567890123456789XY";
	jwt_t *jwt = NULL;
	int    ret = 0;
	char * out;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_HS512, key512, sizeof(key512));
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_NONE, NULL, 0);
	ck_assert_int_eq(ret, 0);

	out = jwt_encode_str(jwt);
	ck_assert_ptr_ne(out, NULL);

	ck_assert_str_eq(out, res);

	jwt_free_str(out);

	jwt_free(jwt);
}
END_TEST

START_TEST(test_jwt_encode_invalid)
{
	unsigned char key512[64] = "012345678901234567890123456789XY"
	                           "012345678901234567890123456789XY";
	jwt_t *jwt = NULL;
	int    ret = 0;

	ALLOC_JWT(&jwt);

	ret = jwt_add_grant(jwt, "iss", "files.cyphre.com");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "sub", "user0");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant(jwt, "ref", "XXXX-YYYY-ZZZZ-AAAA-CCCC");
	ck_assert_int_eq(ret, 0);

	ret = jwt_add_grant_int(jwt, "iat", TS_CONST);
	ck_assert_int_eq(ret, 0);

	ret = jwt_set_alg(jwt, JWT_ALG_HS512, NULL, 0);
	ck_assert_int_eq(ret, EINVAL);

	ret = jwt_set_alg(jwt, JWT_ALG_HS512, NULL, sizeof(key512));
	ck_assert_int_eq(ret, EINVAL);

	ret = jwt_set_alg(jwt, JWT_ALG_HS512, key512, 0);
	ck_assert_int_eq(ret, EINVAL);

	ret = jwt_set_alg(jwt, JWT_ALG_NONE, key512, sizeof(key512));
	ck_assert_int_eq(ret, EINVAL);

	ret = jwt_set_alg(jwt, JWT_ALG_NONE, key512, 0);
	ck_assert_int_eq(ret, EINVAL);

	ret = jwt_set_alg(jwt, JWT_ALG_NONE, NULL, sizeof(key512));
	ck_assert_int_eq(ret, EINVAL);

	/* Set a value that will never happen. */
	ret = jwt_set_alg(jwt, 999, NULL, 0);
	ck_assert_int_eq(ret, EINVAL);

	jwt_free(jwt);
}
END_TEST

static Suite *libjwt_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("LibJWT Encode");

	tc_core = tcase_create("jwt_encode");

	tcase_add_test(tc_core, test_jwt_encode_fp);
	tcase_add_test(tc_core, test_jwt_encode_str);
	tcase_add_test(tc_core, test_jwt_encode_alg_none);
	tcase_add_test(tc_core, test_jwt_encode_hs256);
	tcase_add_test(tc_core, test_jwt_encode_hs384);
	tcase_add_test(tc_core, test_jwt_encode_hs512);
	tcase_add_test(tc_core, test_jwt_encode_change_alg);
	tcase_add_test(tc_core, test_jwt_encode_invalid);

	tcase_set_timeout(tc_core, 30);

	suite_add_tcase(s, tc_core);

	return s;
}

int main(int argc, char *argv[])
{
	int      number_failed;
	Suite *  s;
	SRunner *sr;

	s  = libjwt_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
