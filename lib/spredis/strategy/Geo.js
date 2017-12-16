const Range = require('./Range');
const _ = require('lodash');

const VALID_UNIT = {
	'km': 1,
	'mi': 1,
	'm': 1,
	'f': 1
}
module.exports = class GeoStrategy extends Range {
	_exists(val) {
		if (val === null || val === undefined || val === '') return false;
		if (_.isArray(val)) {
			return val[0] !== null && val[0] !== undefined && val[0] !== '' && val[1] !== null && val[1] !== undefined && val[1] !== '';
		}
		return false;
	}



	getQueryValue(val) {
		let query = {};
		if (_.isString(val)) throw new Error(`Invalid query type for geo field (string:${val})`);
		if (_.isArray(val)) throw new Error(`Invalid query type for geo field (array:${val})`);
		if (!this._exists(val.from)) throw new Error(`Geo radius query missing from ${val}`);
		query.from = val.from;
		query.radius = val.radius ? parseFloat(val.radius) : '+inf';
		if (query.radius !== '+inf' && isNaN(query.radius)) throw new Error(`Geo search error, could not parse radius: ${val}`);
		query.unit = (val.unit || 'km').toLowerCase();
		if (!VALID_UNIT[query.unit]) throw new Error(`Geo search error: invalid unit (${val.unit})`);
		return  query;
	}

	get lang() {
		return 'G';
	}

	set lang(l) {

	}

	setForSingleValue(pipe, val, lang, hint, cleanUp) {
		// console.log('querying for geo')

		let query = this.getQueryValue(val);
		let store = this.getTempIndexName();
		cleanUp.push(store);
		let indexName = this.indexForValue(val, lang);

		if (hint) {
			let hintStore = this.getTempIndexName();
			pipe.zinterstore(hintStore, 2, indexName, hint, 'WEIGHTS', 1, 0);
			pipe.georadius(hintStore, query.from[1], query.from[0], query.radius, query.unit, 'STOREDIST', store);
			cleanUp.push(hintStore);
			return store
		}
		pipe.georadius(indexName, query.from[1], query.from[0], query.radius, query.unit, 'STOREDIST', store);
		return store;
	}

	async unIndexField(pipe, id, val, pos) {
		var exists = this._exists(val);

		if (exists) {
			var indexName = this.indexForValue(val);
			pipe.zremBuffer(indexName, this.toLex(id, pos));
		}
		
		return []
	}

	async indexField(pipe, id, val, pos) {
		var exists = this._exists(val);
		if (exists) {
			var rank = this.toRank(val);
			var indexName = this.indexForValue(val);
			pipe.geoaddBuffer(indexName, rank[1], rank[0], this.toLex(id, pos));
		}
		
		return []
	}
}