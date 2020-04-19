/**
 * @license   http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author    Volker Theile <volker.theile@openmediavault.org>
 * @author    OpenMediaVault Plugin Developers <plugins@omv-extras.org>
 * @copyright Copyright (c) 2009-2013 Volker Theile
 * @copyright Copyright (c) 2013-2020 OpenMediaVault Plugin Developers
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
// require("js/omv/form/plugin/LinkedFields.js")
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/form/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")

Ext.define("OMV.module.admin.system.omvextras.Docker", {
    extend: "OMV.workspace.form.Panel",

    rpcService: "OmvExtras",
    rpcGetMethod: "getDocker",
    rpcSetMethod: "setDocker",

    plugins: [{
        ptype: "linkedfields",
        correlations: [{
            conditions: [
                { name: "portainerStatus2", value: true }
            ],
            properties: function(valid, field) {
                this.setButtonDisabled("portainerweb", !valid);
            }
        },{
            conditions: [{
                name: "advanced", value: false
            }],
            name: ["webport","agentport"],
            properties: ["!show"]
        }]
    }],

    getButtonItems: function() {
        var me = this;
        var items = me.callParent(arguments);
        items.push({
            id: me.getId() + "-docker",
            xtype: "button",
            text: "Docker",
            scope: me,
            icon: "images/refresh.png",
            menu: [{
                text: _("Install"),
                icon: "images/add.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "docker_install" ])
            },{
                text: _("Remove"),
                icon: "images/minus.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "docker_remove" ])
            },{
                text: _("Restart"),
                icon: "images/refresh.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "docker_restart" ])
            }]
        },{
            id: me.getId() + "-portainer",
            xtype: "button",
            text: "Portainer",
            scope: me,
            icon: "images/refresh.png",
            menu: [{
                text: _("Install"),
                icon: "images/add.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "portainer_install" ])
            },{
                text: _("Remove"),
                icon: "images/minus.png",
                handler: Ext.Function.bind(me.onCommandButton, me, [ "portainer_remove" ])
            }]
        },{
            id: me.getId() + "-portainerweb",
            xtype: "button",
            text: _("Open web"),
            icon: "images/arrow-up.png",
            iconCls: Ext.baseCSSPrefix + "btn-icon-16x16",
            disabled: true,
            scope: me,
            handler: function() {
                var port = this.getForm().findField("webport").getValue()
                if (port >= 1000) {
                    window.open("http://" + location.hostname + ":" + port, "_blank");
                }
            }
        });
        return items;
    },

    getFormItems: function () {
        var me = this;
        return [{
            xtype: "fieldset",
            title: _("Docker"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "textfield",
                name: "dockerStorage",
                fieldLabel: _("Docker Storage"),
                allowBlank: true,
                value: "/var/lib/docker",
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("Path to Docker images and containers storage. Leave blank to use custom /etc/docker/daemon.json.")
                }]
            },{
                xtype: "textfield",
                name: "dockerStatus",
                fieldLabel: _("Status"),
                submitValue: false
            },{
                xtype: "textfield",
                name: "dockerVersion",
                fieldLabel: _("Version"),
                submitValue: false
            },{
                border: false,
                html: "<ul>" +
                        "<li>" + _("Install Docker will install the docker-compose package. Location: /usr/bin/ on all systems.") + "</li>" +
                        "<li>" + _("Install Docker will download and place a newer docker-compose in /usr/local/bin/ on 64 bit x86 systems only.") + "</li>" +
                        "<li>" + _("Install Docker will restart the docker service if daemon.json changes.") + "</li>" +
                        "<li>" + _("Remove Docker will remove the docker-compose package.") + "</li>" +
                        "<li>" + _("Remove Docker will delete docker-compose from /usr/local/bin/.") + "</li>" +
                        "<li>" + _("Save will rewrite daemon.json and restart docker if Docker Storage path has changed.") + "</li>" +
                      "</ul>"
            }]
        },{
            xtype: "fieldset",
            title: _("Portainer"),
            fieldDefaults: {
                labelSeparator: ""
            },
            items: [{
                xtype: "checkbox",
                name: "optout",
                fieldLabel: _("Opt-out"),
                checked: true,
                boxLabel: _("This will opt-out of Portainer's minimal analytics.")
            },{
                xtype: "textfield",
                name: "portainerStatus",
                fieldLabel: _("Status"),
                submitValue: false
            },{
                xtype: "checkbox",
                name: "advanced",
                fieldLabel: _("Advanced"),
                submitValue: false
            },{
                xtype: "numberfield",
                name: "webport",
                fieldLabel: _("Web port"),
                vtype: "port",
                minValue: 1000,
                maxValue: 65535,
                allowDecimals: false,
                allowBlank: false,
                value: 9000,
                hidden: true,
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("If port is changed, click Install in the Portainer menu again.")
                }]
            },{
                xtype: "numberfield",
                name: "agentport",
                fieldLabel: _("Agent port"),
                vtype: "port",
                minValue: 1000,
                maxValue: 65535,
                allowDecimals: false,
                allowBlank: false,
                value: 8000,
                hidden: true,
                plugins: [{
                    ptype: "fieldinfo",
                    text: _("If port is changed, click Install in the Portainer menu again.")
                }]
            },{
                xtype: "checkbox",
                name: "portainerStatus2",
                hidden: true,
                submitValue: false
            },{
                border: false,
                html: "<ul>" +
                        "<li>" + _("Install Portainer will install the docker-ce package if not already installed.") + "</li>" +
                        "<li>" + _("Install Portainer will update Portainer to the latest image if the image already exists.") + "</li>" +
                        "<li>" + _("Portainer will listen on port 9000 for the web interface and 8000 for the agent unless changed.") + "</li>" +
                        "<li>" + _("Remove Portainer will remove the Portainer image and container but the volume will not be removed.") + "</li>" +
                      "</ul>"
            }]
        }];
    },

    onCommandButton: function(command) {
        var me = this;
        me.doSubmit();
        var msg = "";
        switch (command) {
            case "update":
                msg = _("Restarting docker ...");
                break;
            case "docker_install":
            case "docker_remove":
            case "portainer_install":
            case "portainer_remove":
                str = command.split("_");
                if (str[1] == "remove") {
                  action = _("Removing");
                } else {
                  action = _("Installing");
                }
                msg = action + " " + str[0] + " ...";
                break;
        }
        var wnd = Ext.create("OMV.window.Execute", {
            title: msg,
            rpcService: "OmvExtras",
            rpcMethod: "doCommand",
            rpcParams: {
                "command": command
            },
            rpcIgnoreErrors: true,
            hideStartButton: true,
            hideStopButton: true,
            listeners: {
                scope: me,
                finish: function(wnd, response) {
                    wnd.appendValue(_("Done."));
                    wnd.setButtonDisabled("close", false);
                },
                exception: function(wnd, error) {
                    OMV.MessageBox.error(null, error);
                    wnd.setButtonDisabled("close", false);
                },
                close: function() {
                    me.doReload();
                }
            }
        });
        wnd.setButtonDisabled("close", true);
        wnd.show();
        wnd.start();
    }
});

OMV.WorkspaceManager.registerPanel({
    id: "docker",
    path: "/system/omvextras",
    text: "Docker",
    position: 20,
    className: "OMV.module.admin.system.omvextras.Docker"
});
