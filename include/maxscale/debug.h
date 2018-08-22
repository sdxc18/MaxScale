/*
 * Copyright (c) 2018 MariaDB Corporation Ab
 *
 * Use of this software is governed by the Business Source License included
 * in the LICENSE.TXT file and at www.mariadb.com/bsl11.
 *
 * Change Date: 2022-01-01
 *
 * On the date above, in accordance with the Business Source License, use
 * of this software will be governed by version 2 or later of the General
 * Public License.
 */
 #pragma once

#include <maxscale/cdefs.h>
#include <maxbase/assert.h>
#include <maxscale/log.h>

MXS_BEGIN_DECLS

#define ss_dassert      mxb_assert
#define ss_info_dassert mxb_assert_message

MXS_END_DECLS
