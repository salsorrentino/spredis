const Strategy = require('./Strategy');
var _ = require('lodash');

module.exports = class RangeStrategy extends Strategy {
	getQueryValue(val, min) {
		if (val === null || val === undefined || val === '') {
			return min ? '-inf' : '+inf';
		}
		return this.actualValue(val);
	}

	get lang() {
		return 'M';
	}

	set lang(l) {

	}
	
	sortIndex(lang) {
		return this.indexName + ':SALL:' + this.lang;
	}

	indexForValue(val, lang) {
		//ignore language for number type fields, will alway be 'M' for
		return this.indexName + ':V:' + this.lang;
	}

	setForSingleValue(pipe, val, lang, hint, cleanUp) {
		// if (val === true) {
		// 	index: this.indexForValue(val);
		// }
		if (_.isString(val) || _.isNumber(val)) {
			val = {min:val, max:val};
		}
		if (val.min === null && val.max === null) return hint;
		val = {min:this.getQueryValue(val.min,true), max:this.getQueryValue(val.max, false)};
		let store = this.getTempIndexName();
		let indexName = this.indexForValue(val, lang);
		cleanUp.push(store);
		if (hint) {
			let hintStore = this.getTempIndexName();
			pipe.zinterstore(hintStore, 2, indexName, hint, 'WEIGHTS', 1, 0);
			pipe.storeRange(hintStore, store, val.min, val.max, indexName, hint);
			cleanUp.push(hintStore);
			return store
		}
		pipe.storeRange(indexName, store, val.min, val.max);
		return store;	
	}
}