/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
 *
* This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/MessageBox.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/form/field/Password.js")
// require("js/omv/util/Format.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Repos", {
	extend: "OMV.workspace.grid.Panel",
	requires: [
		"OMV.data.Store",
		"OMV.data.Model",
		"OMV.data.proxy.Rpc",
		"OMV.util.Format"
	],

	stateful: true,
	stateId: "a01bda0e-3f21-11e2-8510-74221868ca88",
	bodyCls: "x-grid-without-dirty-cell",
	features: [{
		ftype: "grouping",
		groupHeaderTpl: "{name}"
	}],
	columns: [{
		text: _("Type"),
		sortable: true,
		groupable: true,
		hidden: true,
		dataIndex: "type",
		stateId: "type",
		align: "center",
		renderer: function(value, metaData, record, rowIndex, colIndex,
		  store, view) {
			return _(value);
		}
	},{
		xtype: "checkcolumn",
		text: _("Enable"),
		groupable: false,
		dataIndex: "enable",
		stateId: "enable",
		align: "center",
		width: 70,
		resizable: false
	},{
		text: _("Repository"),
		sortable: true,
		groupable: false,
		dataIndex: "repo",
		stateId: "repo",
		flex: 1,
		renderer: function(value, metaData, record, rowIndex, colIndex,
		  store, view) {
			return _(value);
		}
	},{
		text: _("Description"),
		sortable: true,
		groupable: false,
		dataIndex: "description",
		stateId: "description",
		flex: 1,
		renderer: function(value, metaData, record, rowIndex, colIndex,
		  store, view) {
			return _(value);
		}
	}],

	hideAddButton     : true,
	hideEditButton    : true,
	hideDeleteButton  : true,
	hideApplyButton   : false,
	hideRefreshButton : false,

	initComponent: function() {
		var me = this;
		Ext.apply(me, {
			store: Ext.create("OMV.data.Store", {
				autoLoad   : true,
				groupField : "type",
				model      : OMV.data.Model.createImplicit({
					idProperty  : "id",
					fields      : [
						{ name : "id", type : "string" },
						{ name : "type", type : "string" },
                        { name : "enable", type : "boolean" },
						{ name : "repo", type : "string" },
						{ name : "description", type : "string" }
					]
				}),
				proxy : {
					type             : "rpc",
					appendSortParams : false,
					rpcData          : {
						service : "omvextrasorg",
						method  : "getSecondaryList"
					}
				},
				sorters : [{
					direction : "ASC",
					property  : "repo"
				}]
			})
		});
		me.callParent(arguments);
	},

	onApplyButton: function() {
		var me = this;
		var records = me.store.getRange();
		var params = [];
		Ext.Array.each(records, function(record) {
			params.push({
				  "id"     : record.get("id"),
				  "enable" : record.get("enable")
			  });
		});
		// Execute RPC.
		OMV.Rpc.request({
			  scope: me,
			  callback: function(id, success, response) {
				  this.store.reload();
			  },
			  relayErrors: false,
			  rpcData: {
				  service : "omvextrasorg",
				  method  : "set",
				  params  : params
			  }
		  });
	}
});

OMV.WorkspaceManager.registerPanel({
	id        : "repos",
	path      : "/system/omvextrasorg",
	text      : _("Repos"),
	position  : 21,
	className : "OMV.module.admin.system.omvextrasorg.Repos"
});
