/**
 *    Copyright (C) 2018-present MongoDB, Inc.
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

#pragma once

#include "mongo/db/op_observer/op_observer.h"
#include "mongo/db/op_observer/op_observer_util.h"
#include "mongo/db/s/collection_sharding_state.h"

namespace mongo {
namespace repl {

class ReplOperation;

}  // namespace repl

class ShardingWriteRouter;

class OpObserverImpl : public OpObserver {
    OpObserverImpl(const OpObserverImpl&) = delete;
    OpObserverImpl& operator=(const OpObserverImpl&) = delete;

public:
    OpObserverImpl() = default;
    virtual ~OpObserverImpl() = default;

    void onCreateIndex(OperationContext* opCtx,
                       const NamespaceString& nss,
                       const UUID& uuid,
                       BSONObj indexDoc,
                       bool fromMigrate) final;

    void onStartIndexBuild(OperationContext* opCtx,
                           const NamespaceString& nss,
                           const UUID& collUUID,
                           const UUID& indexBuildUUID,
                           const std::vector<BSONObj>& indexes,
                           bool fromMigrate) final;
    void onStartIndexBuildSinglePhase(OperationContext* opCtx, const NamespaceString& nss) final;
    void onAbortIndexBuildSinglePhase(OperationContext* opCtx, const NamespaceString& nss) final;

    void onCommitIndexBuild(OperationContext* opCtx,
                            const NamespaceString& nss,
                            const UUID& collUUID,
                            const UUID& indexBuildUUID,
                            const std::vector<BSONObj>& indexes,
                            bool fromMigrate) final;

    void onAbortIndexBuild(OperationContext* opCtx,
                           const NamespaceString& nss,
                           const UUID& collUUID,
                           const UUID& indexBuildUUID,
                           const std::vector<BSONObj>& indexes,
                           const Status& cause,
                           bool fromMigrate) final;

    void onInserts(OperationContext* opCtx,
                   const NamespaceString& nss,
                   const UUID& uuid,
                   std::vector<InsertStatement>::const_iterator begin,
                   std::vector<InsertStatement>::const_iterator end,
                   bool fromMigrate) final;
    void onUpdate(OperationContext* opCtx, const OplogUpdateEntryArgs& args) final;
    void aboutToDelete(OperationContext* opCtx,
                       const NamespaceString& nss,
                       const UUID& uuid,
                       const BSONObj& doc) final;
    void onDelete(OperationContext* opCtx,
                  const NamespaceString& nss,
                  const UUID& uuid,
                  StmtId stmtId,
                  const OplogDeleteEntryArgs& args) final;
    void onInternalOpMessage(OperationContext* opCtx,
                             const NamespaceString& nss,
                             const boost::optional<UUID>& uuid,
                             const BSONObj& msgObj,
                             boost::optional<BSONObj> o2MsgObj,
                             boost::optional<repl::OpTime> preImageOpTime,
                             boost::optional<repl::OpTime> postImageOpTime,
                             boost::optional<repl::OpTime> prevWriteOpTimeInTransaction,
                             boost::optional<OplogSlot> slot) final;
    void onCreateCollection(OperationContext* opCtx,
                            const CollectionPtr& coll,
                            const NamespaceString& collectionName,
                            const CollectionOptions& options,
                            const BSONObj& idIndex,
                            const OplogSlot& createOpTime,
                            bool fromMigrate) final;
    void onCollMod(OperationContext* opCtx,
                   const NamespaceString& nss,
                   const UUID& uuid,
                   const BSONObj& collModCmd,
                   const CollectionOptions& oldCollOptions,
                   boost::optional<IndexCollModInfo> indexInfo) final;
    void onDropDatabase(OperationContext* opCtx, const DatabaseName& dbName) final;
    repl::OpTime onDropCollection(OperationContext* opCtx,
                                  const NamespaceString& collectionName,
                                  const UUID& uuid,
                                  std::uint64_t numRecords,
                                  CollectionDropType dropType) final;
    repl::OpTime onDropCollection(OperationContext* opCtx,
                                  const NamespaceString& collectionName,
                                  const UUID& uuid,
                                  std::uint64_t numRecords,
                                  CollectionDropType dropType,
                                  bool markFromMigrate) final;
    void onDropIndex(OperationContext* opCtx,
                     const NamespaceString& nss,
                     const UUID& uuid,
                     const std::string& indexName,
                     const BSONObj& indexInfo) final;
    repl::OpTime preRenameCollection(OperationContext* opCtx,
                                     const NamespaceString& fromCollection,
                                     const NamespaceString& toCollection,
                                     const UUID& uuid,
                                     const boost::optional<UUID>& dropTargetUUID,
                                     std::uint64_t numRecords,
                                     bool stayTemp) final;
    repl::OpTime preRenameCollection(OperationContext* opCtx,
                                     const NamespaceString& fromCollection,
                                     const NamespaceString& toCollection,
                                     const UUID& uuid,
                                     const boost::optional<UUID>& dropTargetUUID,
                                     std::uint64_t numRecords,
                                     bool stayTemp,
                                     bool markFromMigrate) final;
    void postRenameCollection(OperationContext* opCtx,
                              const NamespaceString& fromCollection,
                              const NamespaceString& toCollection,
                              const UUID& uuid,
                              const boost::optional<UUID>& dropTargetUUID,
                              bool stayTemp) final;
    void onRenameCollection(OperationContext* opCtx,
                            const NamespaceString& fromCollection,
                            const NamespaceString& toCollection,
                            const UUID& uuid,
                            const boost::optional<UUID>& dropTargetUUID,
                            std::uint64_t numRecords,
                            bool stayTemp) final;
    void onRenameCollection(OperationContext* opCtx,
                            const NamespaceString& fromCollection,
                            const NamespaceString& toCollection,
                            const UUID& uuid,
                            const boost::optional<UUID>& dropTargetUUID,
                            std::uint64_t numRecords,
                            bool stayTemp,
                            bool markFromMigrate) final;
    void onImportCollection(OperationContext* opCtx,
                            const UUID& importUUID,
                            const NamespaceString& nss,
                            long long numRecords,
                            long long dataSize,
                            const BSONObj& catalogEntry,
                            const BSONObj& storageMetadata,
                            bool isDryRun) final;
    void onApplyOps(OperationContext* opCtx,
                    const DatabaseName& dbName,
                    const BSONObj& applyOpCmd) final;
    void onEmptyCapped(OperationContext* opCtx,
                       const NamespaceString& collectionName,
                       const UUID& uuid) final;
    void onUnpreparedTransactionCommit(OperationContext* opCtx,
                                       std::vector<repl::ReplOperation>* statements,
                                       size_t numberOfPrePostImagesToWrite) final;
    void onBatchedWriteStart(OperationContext* opCtx) final;
    void onBatchedWriteCommit(OperationContext* opCtx) final;
    void onBatchedWriteAbort(OperationContext* opCtx) final;
    void onPreparedTransactionCommit(
        OperationContext* opCtx,
        OplogSlot commitOplogEntryOpTime,
        Timestamp commitTimestamp,
        const std::vector<repl::ReplOperation>& statements) noexcept final;

    std::unique_ptr<ApplyOpsOplogSlotAndOperationAssignment> preTransactionPrepare(
        OperationContext* opCtx,
        const std::vector<OplogSlot>& reservedSlots,
        size_t numberOfPrePostImagesToWrite,
        Date_t wallClockTime,
        std::vector<repl::ReplOperation>* statements) final;

    void onTransactionPrepare(
        OperationContext* opCtx,
        const std::vector<OplogSlot>& reservedSlots,
        std::vector<repl::ReplOperation>* statements,
        const ApplyOpsOplogSlotAndOperationAssignment* applyOpsOperationAssignment,
        size_t numberOfPrePostImagesToWrite,
        Date_t wallClockTime) final;
    void onTransactionAbort(OperationContext* opCtx,
                            boost::optional<OplogSlot> abortOplogEntryOpTime) final;
    void onMajorityCommitPointUpdate(ServiceContext* service,
                                     const repl::OpTime& newCommitPoint) final {}

private:
    virtual void shardObserveAboutToDelete(OperationContext* opCtx,
                                           NamespaceString const& nss,
                                           BSONObj const& doc) {}
    virtual void shardObserveInsertOp(OperationContext* opCtx,
                                      const NamespaceString nss,
                                      const BSONObj& insertedDoc,
                                      const repl::OpTime& opTime,
                                      const ShardingWriteRouter& shardingWriteRouter,
                                      const bool fromMigrate,
                                      const bool inMultiDocumentTransaction) {}
    virtual void shardObserveUpdateOp(OperationContext* opCtx,
                                      const NamespaceString nss,
                                      boost::optional<BSONObj> preImageDoc,
                                      const BSONObj& postImageDoc,
                                      const repl::OpTime& opTime,
                                      const ShardingWriteRouter& shardingWriteRouter,
                                      const repl::OpTime& prePostImageOpTime,
                                      const bool inMultiDocumentTransaction) {}
    virtual void shardObserveDeleteOp(OperationContext* opCtx,
                                      const NamespaceString nss,
                                      const BSONObj& documentKey,
                                      const repl::OpTime& opTime,
                                      const ShardingWriteRouter& shardingWriteRouter,
                                      const repl::OpTime& preImageOpTime,
                                      const bool inMultiDocumentTransaction) {}
    virtual void shardObserveTransactionPrepareOrUnpreparedCommit(
        OperationContext* opCtx,
        const std::vector<repl::ReplOperation>& stmts,
        const repl::OpTime& prepareOrCommitOptime) {}
    void _onReplicationRollback(OperationContext* opCtx, const RollbackObserverInfo& rbInfo) final;
};

}  // namespace mongo
