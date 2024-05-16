#pragma once

#include <tom/migration.hpp>

namespace Migrations
{

    struct CreatePostsTable : Migration
    {
        T_MIGRATION

        void up() const override
        {
            Schema::create("auditorium", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_cinema");
                table.string("name", 45);
                table.integer("room");
                table.integer("type_ims");
                table.string("ip_ims", 45);
                table.integer("port_ims");
                table.string("user_ims", 45);
                table.string("pass_ims", 45);

                table.unique("ip_ims");
            });
            Schema::create("cinema", [](Blueprint &table)
            {
                table.id().from(4);
                table.string("name", 45);

                table.unique("name");
            });
            Schema::create("cpl", [](Blueprint &table)
            {
                table.id().from(4);
                table.string("uuid", 45);
                table.string("name", 200);
                table.integer("type_cpl"); 
                table.string("sha1_sync", 40);
                table.string("path_cpl", 200);
                table.string("path_sync", 200).nullable();

                table.unique("uuid");
            });
            Schema::create("feature", [](Blueprint &table)
            {
                table.id().from(4);
                table.string("name", 45);

                table.unique("name");
            });

            Schema::create("group", [](Blueprint &table)
            {
                table.id().from(4);
                table.string("name", 45);
                table.string("description", 200).nullable();
                
                table.unique("name");
            });
            Schema::create("link_cinema_group", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_cinema");
                table.integer("id_group");

            });
            Schema::create("link_cinema_release", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_release");
                table.integer("id_cinema");

            });
            Schema::create("link_cpl_release", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_release");
                table.integer("id_cpl");

            });
            Schema::create("link_group_release", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_release");
                table.integer("id_group");

            });
            Schema::create("link_script_release", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_release");
                table.integer("id_script");

            });
            Schema::create("link_cpl_script", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_cpl");
                table.integer("id_script");

            });
            Schema::create("release", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_feature");
                table.date("date_in");
                table.date("date_out").nullable();
                table.string("name", 45);
                table.integer("id_parent_release").nullable();
                table.string("global_pattern", 45);
                table.string("specific_pattern", 45);
                table.integer("finalized").defaultValue(0);
            });
            Schema::create("release_cut", [](Blueprint &table)
            {
                table.id().from(4);
                table.integer("id_release");
                table.string("decription", 200).nullable();
                table.integer("position");
                table.integer("size");

            });
            Schema::create("script", [](Blueprint &table)
            {
                table.id().from(4);
                table.string("name", 45);
                table.string("path", 200);
                table.string("cis_name", 45);
                table.string("lvi_name", 45);
                table.integer("type");
                table.string("version", 45);
                table.string("sha1Lvi", 40);

            });
            Schema::create("server", [](Blueprint &table)
            {
                table.id().from(7);
                table.integer("id_auditorium");
                table.integer("type_server");
                table.ipAddress("ip");

                table.unique("ip");
            });
        }

        void down() const override
        {
            Schema::dropIfExists("auditorium");
            Schema::dropIfExists("cinema");
            Schema::dropIfExists("cpl");
            Schema::dropIfExists("feature");
            Schema::dropIfExists("group");
            Schema::dropIfExists("link_cinema_group");
            Schema::dropIfExists("link_cinema_release");
            Schema::dropIfExists("link_cpl_release");
            Schema::dropIfExists("link_group_release");
            Schema::dropIfExists("link_script_release");
            Schema::dropIfExists("link_cpl_script");
            Schema::dropIfExists("release");
            Schema::dropIfExists("release_cut");
            Schema::dropIfExists("script");
            Schema::dropIfExists("server");
        }
    };

}