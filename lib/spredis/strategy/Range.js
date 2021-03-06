/*
	a number or date field.
	../types/[type] will handle specific data transformation based on the exact field type
*/

const Strategy = require('./Strategy');
var _ = require('lodash');

module.exports = class RangeStrategy extends Strategy {
	getQueryValue(val, min) {
		if (val === null || val === undefined || val === '') {
			return min ? '(-inf' : '+inf';
		}
		return this.actualValue(val);
	}
	
	toCompositeQuery(val) {
		if (_.isString(val) || _.isNumber(val)) {
			let v = this.getQueryValue(val);
			return [v,v,0,0];
		}
		let min = this.getQueryValue(val.min, true);
		let max = this.getQueryValue(val.max);
		let minInf = min == '(-inf';
		// console.log( ...[minInf ? '-inf' : min ,max, minInf ? 1 : 0 , 0] );
		return [minInf ? '-inf' : min ,max, minInf ? 1 : 0 , 0];
	}


	setForSingleValue(pipe, val, lang, hint, cleanUp) {
		let indexName = this.indexForValue(val, lang);
		let store;
		if (_.isString(val) || _.isNumber(val) || val === true || val === false) {
			store = this.getTempIndexName(cleanUp)
			pipe.call('spredis.zlinkset', indexName, store, this.getQueryValue(val));
			if (hint) {
				let store2 = this.getTempIndexName(cleanUp);
				pipe.call('spredis.stinterstore', store2, store, hint);
				return store2;
			}
			return store;
		}
		if (val.min === null && val.max === null) return hint;
		val = {min:this.getQueryValue(val.min,true), max:this.getQueryValue(val.max, false)};
		store = this.getTempIndexName(cleanUp);
		pipe.call('spredis.storerangebyscore', store, indexName, hint || '' , val.min, val.max);
		return store;	
	}



	unIndexField(pipe, id, val, pos, lang='en') {
		let exists = this._exists(val);
		let indexName = this.indexForValue(val, lang);
		let rank = exists ? this.toRank(val) : '-inf';
		pipe.call('spredis.zrem', indexName, this.toLex(id, pos), rank);
		if (this.type.supportsFacet && this.type.facet) {
			pipe.call('spredis.hdel',this.facetName(lang), id, pos);
		}
	}

	indexField(pipe, id, val, pos, lang='en') {
		let exists = this._exists(val);
		let rank = exists ? this.toRank(val) : '-inf';
		let indexName = this.indexForValue(val, lang);
		pipe.call('spredis.zadd',indexName, this.toLex(id, pos), rank);
		if (this.type.supportsFacet && this.type.facet) {
			let av = this.actualValue(val);
			if (!isNaN(av)) pipe.call('spredis.hsetdbl',this.facetName(lang), id, pos, this.actualValue(val));
		}
		return 1
	}
}