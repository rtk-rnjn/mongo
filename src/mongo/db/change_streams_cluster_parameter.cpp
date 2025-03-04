/**
 *    Copyright (C) 2022-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#define MONGO_LOGV2_DEFAULT_COMPONENT ::mongo::logv2::LogComponent::kQuery

#include "mongo/db/change_streams_cluster_parameter.h"

#include "mongo/base/status.h"
#include "mongo/db/change_streams_cluster_parameter_gen.h"
#include "mongo/logv2/log.h"
namespace mongo {

Status validateChangeStreamsClusterParameter(
    const ChangeStreamsClusterParameterStorage& clusterParameter) {
    LOGV2_DEBUG(6594801,
                1,
                "Validating change streams cluster parameter",
                "enabled"_attr = clusterParameter.getEnabled(),
                "expireAfterSeconds"_attr = clusterParameter.getExpireAfterSeconds());
    if (clusterParameter.getEnabled()) {
        if (clusterParameter.getExpireAfterSeconds() <= 0) {
            return Status(ErrorCodes::BadValue,
                          "Expected a positive integer for 'expireAfterSeconds' field if 'enabled' "
                          "field is true");
        }
    } else {
        if (clusterParameter.getExpireAfterSeconds() != 0) {
            return Status(
                ErrorCodes::BadValue,
                "Expected a zero value for 'expireAfterSeconds' if 'enabled' field is false");
        }
    }
    return Status::OK();
}

}  // namespace mongo
