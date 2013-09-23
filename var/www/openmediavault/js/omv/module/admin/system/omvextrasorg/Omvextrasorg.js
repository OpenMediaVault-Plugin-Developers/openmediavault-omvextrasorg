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
// require("js/omv/workspace/panel/Panel.js")

Ext.define("OMV.module.admin.system.omvextrasorg.Info", {
	extend: "OMV.workspace.panel.Panel",

	initComponent: function() {
		var me = this;
		me.html = "<form style='overflow: auto; height: 100%;'>";
		me.html += me.createBox("The OMV-Extras.org plugin is installed.");
		me.html += "</form>";
		me.callParent(arguments);
	},

	createBox: function(msg) {
		return [ '<div class="x-box-aboutbox">', msg, '</div>' ].join('');
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
