/**
 *
 * @license     http://www.gnu.org/licenses/gpl.html GPL Version 3
 * @author      Volker Theile <volker.theile@openmediavault.org>
 * @author      Aaron Murray <aaron@omv-extras.org>
 * @author      Ian Moore <imooreyahoo@gmail.com>
 * @author      Marcel Beck <marcel.beck@mbeck.org>
 * @copyright   Copyright (c) 2009-2013 Volker Theile
 * @copyright   Copyright (c) 2011 Ian Moore
 * @copyright   Copyright (c) 2012 Marcel Beck
 * @copyright   Copyright (c) 2013-2014 OpenMediaVault Plugin Developers
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
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/proxy/Rpc.js")

Ext.define("OMV.module.admin.diagnostic.system.ProcessList", {
    extend   : "OMV.workspace.grid.Panel",
    requires : [
        "OMV.data.Store",
        "OMV.data.proxy.Rpc"
    ],

    constructor : function(config) {
        var me = this;
        config = Ext.apply({
            hideAddButton     : true,
            hideEditButton    : true,
            hideDeleteButton  : true,
            hideRefreshButton : false,
            hidePagingToolbar : true,
            disableSelection  : true,
            stateful          : true,
            stateId           : "976130ef-a647-40e8-9b09-02ced906680a",
            columns           : [{
                header    : _("PID"),
                sortable  : true,
                dataIndex : "PID",
                id        : "PID",
                width     : 60
            },{
                header    : _("User"),
                sortable  : true,
                dataIndex : "USER",
                id        : "USER",
                width     : 80
            },{
                header    : _("% CPU"),
                sortable  : true,
                dataIndex : "CPU",
                id        : "CPU",
                width     : 80
            },{
                header    : _("% MEM"),
                sortable  : true,
                dataIndex : "MEM",
                id        : "MEM",
                width     : 80
            },{
                header    : _("Command"),
                sortable  : true,
                dataIndex : "COMMAND",
                id        : "COMMAND"
            },{
                header    : _("State"),
                sortable  : true,
                dataIndex : "STAT",
                id        : "STAT",
                renderer  : function (val) {
                    /*
                    PROCESS STATE CODES
                    Here are the different values that the s, stat and state output specifiers
                    (header "STAT" or "S") will display to describe the state of a process.
                    D    Uninterruptible sleep (usually IO)
                    R    Running or runnable (on run queue)
                    S    Interruptible sleep (waiting for an event to complete)
                    T    Stopped, either by a job control signal or because it is being traced.
                    W    paging (not valid since the 2.6.xx kernel)
                    X    dead (should never be seen)
                    Z    Defunct ("zombie") process, terminated but not reaped by its parent.
                    */
                    switch (val.substr(0, 1)) {
                        case 'D':
                            return _('Wait');
                        case 'R':
                            return _('Running');
                        case 'S':
                            return _('Sleeping');
                        case 'T':
                            return _('Paused');
                        case 'Z':
                            return _('Zombie');
                    }
                    return val;
                },
                width    :80
            }]
        }, config || {});
        me.callParent([ config ]);
    },

    initComponent: function() {
        var me = this;
        me.store = Ext.create("OMV.data.Store", {
            autoLoad : true,
            fields   : [
                { name : "PID", type : "int" },
                { name : "USER", type : "string" },
                { name : "STAT", type : "string" },
                { name : "CPU", type : "string" },
                { name : "MEM", type : "string" },
                { name : "COMMAND", type : "string" }
            ],
            proxy : Ext.create("OMV.data.proxy.Rpc", {
                appendSortParams : false,
                rpcData          : {
                    service : "OmvExtrasOrg",
                    method  : "getProcessList"
                }
            }),
            sorters : [{
                direction : "ASC",
                property  : "PID"
            }]
        });

        me.callParent(arguments);
    }
});

OMV.WorkspaceManager.registerPanel({
    id        : "processlist",
    path      : "/diagnostic/system",
    text      : _("Process List"),
    position  : 25,
    className : "OMV.module.admin.diagnostic.system.ProcessList"
});
