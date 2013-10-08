/**
 * This file is part of OpenMediaVault.
 *
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 *
 * OpenMediaVault is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * OpenMediaVault is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenMediaVault. If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Info", {
	extend: "OMV.workspace.form.Panel",

	rpcService: "OmvExtrasOrg",
	rpcGetMethod: "getSettings",
	rpcSetMethod: "setSettings",
	
	getFormItems: function() {
		var me = this;
		return [{
			xtype: "fieldset",
			title: _("Repositories"),
			fieldDefaults: {
				labelSeparator: ""
			},
			items: [{
				xtype: "checkbox",
				name: "enable",
				fieldLabel: _("OMV-Extras.org"),
				boxLabel: _("Enable OMV-Extras.org repository"),
				checked: true
			},{
				xtype: "checkbox",
				name: "testing",
				fieldLabel: _("Testing"),
				boxLabel: _("Enable OMV-Extras.org testing repository  (Use at your own risk!)"),
				checked: false
			},{
				xtype: "checkbox",
				name: "vbox",
				fieldLabel: _("Virtualbox"),
				boxLabel: _("Enable Sun's Virtualbox repository  (disable if using armel/armhf)"),
				checked: true
			}]
		}];
	}
});

OMV.WorkspaceManager.registerNode({
	id: "omvextrasorg",
	path: "/system",
	text: _("OMV-Extras.org"),
	icon16: "images/plug.png",
	iconSvg: "images/plug.svg",
	position: 95
});

OMV.WorkspaceManager.registerPanel({
	id: "omvextrasorg",
	path: "/system/omvextrasorg",
	text: _("OMV-Extras.org"),
	position: 10,
	className: "OMV.module.admin.system.omvextrasorg.Info"
});
