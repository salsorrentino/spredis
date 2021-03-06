/*
	an abstract class which all stratgies extend
	defines Indexer/Searcher->Field interaction
*/


const constants = require('../constants');
const uuid = require('uuid').v4;
const ASC = 'ASC';
const DESC = 'DESC';
const _ = require('lodash');
const Prefixed = require('../Prefixed');
const UNIVERSAL_LANG = 'U';

module.exports = class Strategy extends Prefixed {
	constructor(def, prefix) {
		super();
		this.type = def.typeDef;
		this.prefix = prefix;
		this.allIdsKey = prefix + ':ALLIDS';
		this.name = def.name
		this.indexName = def.indexName;

	}

	toIndexableValue(val) {
		return val;
	}

	resolveLang(lang='en') {
		return this.type.universal ? UNIVERSAL_LANG : lang;
	}

	_exists(val) {
		return val !== null && val !== undefined && val !== '';
	}

	_queryExists(val) {
		return this._exists(val);
	}
	get sep() {
		return '';
	}

	get asc() {
		return constants.ASC;
	}

	get desc() {
		return constants.DESC;
	}
	get source() {
		return this.type.source;
	}
	get defaultValue() {
		return this.type.defaultValue;
	}
	
	get supportsSource() {
		return this.type.supportsSource;
	}
	toRank(val) {
		return this.type.toRank(val, this.sortPrecision);
	}

	toLex(id, pos) {
		return this.type.toLex(id, pos);
	}

	toSort(val) {
		return this.type.toSort(val, this.sortPrecision);
	}

	actualValue(val) {
		return this.type.actualValue(val);
	}

	getValFromSource(doc) {
		return null;
	}

	facetName(lang) {
		// lang = this.resolveLang(lang);
		return this.indexName + ':A:' + this.resolveLang(lang);
	}

	sortIndex(lang) {
		return this.indexForValue(null, lang);
	}

	getQueryValue(val) {
		return val;
	}

	indexForValue(val, lang) {
		return this.indexName + ':V:' + this.resolveLang(lang);
	}

	indexForSpecificValue(val) {
		return null;
	}


	get sortPrecision() {
		return this.type ? this.type.sortPrecision : null;
	}


	prefixSearchIndex(lang) {
		return null;
	}

	suffixSearchIndex(lang) {
		return null;
	}

	indexForSearchValue(pipe, val, lang, hint, cleanUp) {
		val =  _.isArray(val) && val.length == 1 ? val[0] : val;
		if (_.isArray(val) && val.length === 0) return hint;
		if (_.isArray(val)) { //handling the ors
			let indeces = [];
			for (var i = 0; i < val.length; i++) {
				let v = val[i]
				let index = this.setForSingleValue(pipe,v, lang, hint, cleanUp);
				indeces.push(index);
			}

			let finalStore  = this.getTempIndexName(cleanUp);
			let len = indeces.length;
			let command = ['spredis.stunionstore',finalStore];
			command.push.apply(command, indeces);
			pipe.call.apply(pipe, command);
			return finalStore
		}

		let index = this.setForSingleValue(pipe, val, lang, hint, cleanUp);
		return index
		
	}

	unIndexField(pipe, id, val, pos, lang) {
		//empty implementation
	}

	indexField(pipe, id, val, pos, lang) {
		//empty implementation
	}

	
}