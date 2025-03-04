// Test that $geoNear requires a geospatial index.
// $geoNear is not allowed in a facet, even in a lookup.
// @tags: [
//   do_not_wrap_aggregations_in_facets,
// ]
(function() {
"use strict";

load("jstests/aggregation/extras/utils.js");  // For "assertErrorCode".

const coll = db.coll;
const from = db.from;

coll.drop();
from.drop();

const geonearPipeline = [
    {$geoNear: {near: [0, 0], distanceField: "distance", spherical: true}},
];

const geonearWithinLookupPipeline = [
        {
          $lookup: {
              pipeline: geonearPipeline,
              from: from.getName(),
              as: "c",
          }
        },
    ];

assert.commandWorked(coll.insert({_id: 5, x: 5}));
assert.commandWorked(from.insert({_id: 1, geo: [0, 0]}));

// Fail without index.
assertErrorCode(from, geonearPipeline, ErrorCodes.IndexNotFound);
assertErrorCode(coll, geonearWithinLookupPipeline, ErrorCodes.IndexNotFound);

assert.commandWorked(from.createIndex({geo: "2dsphere"}));

// Run successfully when you have the geospatial index.
assert.eq(from.aggregate(geonearPipeline).itcount(), 1);
assert.eq(coll.aggregate(geonearWithinLookupPipeline).itcount(), 1);
}());
