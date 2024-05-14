#pragma once
#include <orm/db.hpp>
#include <tom/migration.hpp>

using Orm::DB;

namespace Migrations
{

    struct RecreateDatabaseStructure : Migration
    {
        /*! Filename of the migration file. */
        T_MIGRATION

        /*! Run the migrations. */
        void up() const override
        {
            Schema::create("groups", [](Blueprint &table)
            {
                table.id("id_groups");
            });
            Schema::create("location", [](Blueprint &table)
            {
                table.id("id_location");
            });
            Schema::create("connection", [](Blueprint &table)
            {
                table.id("id_connection");
            });
            Schema::create("site", [](Blueprint &table)
            {
                table.id("id_site");
                table.unsignedBigInteger("id_groups");
                table.unsignedBigInteger("id_location");
                table.unsignedBigInteger("id_connection");

                table.foreign("id_groups").references("id_groups").on("groups");
                table.foreign("id_location").references("id_location").on("location");
                table.foreign("id_connection").references("id_connection").on("connection");
            });
            Schema::create("layouting_configuration", [](Blueprint &table)
            {
                table.id("id_layouting");
                table.unsignedBigInteger("id_site");

                table.foreign("id_site").references("id_site").on("site");
            });
            Schema::create("par", [](Blueprint &table)
            {
                table.id("id_par");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
            });
            Schema::create("par_menage", [](Blueprint &table)
            {
                table.id("id_par_menage");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
            });
            Schema::create("back_light", [](Blueprint &table)
            {
                table.id("id_back_light");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
            });
            Schema::create("mouving_head", [](Blueprint &table)
            {
                table.id("id_mouving_head");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
            });
            Schema::create("panel", [](Blueprint &table)
            {
                table.id("id_panel");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
            });
            Schema::create("default_server_configuration_hallway", [](Blueprint &table)
            {
                table.id("id_default_hallway");
            });
            Schema::create("default_server_configuration_auditorium", [](Blueprint &table)
            {
                table.id("id_default_auditorium");
            });
            Schema::create("projection_sever_configuration", [](Blueprint &table)
            {
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
            });
            Schema::create("hallway_server_configuration", [](Blueprint &table)
            {
                table.unsignedBigInteger("id_layouting");
                table.unsignedBigInteger("id_default_hallway").nullable();
                
                table.foreign("id_default_hallway").references("id_default_hallway").on("default_server_configuration_hallway");
                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
                table.primary("id_layouting");
            });
            Schema::create("auditorium_server_configuration", [](Blueprint &table)
            {
                table.unsignedBigInteger("id_layouting");
                table.unsignedBigInteger("id_default_auditorium").nullable();
                
                table.foreign("id_default_auditorium").references("id_default_auditorium").on("default_server_configuration_auditorium");
                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
                table.primary("id_layouting");
            });
            Schema::create("localisation", [](Blueprint &table)
            {
                table.id("id_localisation");
            });
            Schema::create("type", [](Blueprint &table)
            {
                table.id("id_type");
            });
            Schema::create("movie", [](Blueprint &table)
            {
                table.id("id_movie");
            });
            Schema::create("releases", [](Blueprint &table)
            {
                table.unsignedBigInteger("id_localisation");
                table.unsignedBigInteger("id_type");
                table.unsignedBigInteger("id_movie");

                table.foreign("id_localisation").references("id_localisation").on("localisation");
                table.foreign("id_type").references("id_type").on("type");
                table.foreign("id_movie").references("id_movie").on("movie");
                table.primary({"id_localisation", "id_type", "id_movie"});
            });
            Schema::table("cpl", [](Blueprint &table)
            {
                table.unsignedBigInteger("id_localisation");
                table.unsignedBigInteger("id_type");
                table.unsignedBigInteger("id_movie");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
                table.foreign("id_localisation").references("id_localisation").on("releases");
                table.foreign("id_type").references("id_type").on("releases");
                table.foreign("id_movie").references("id_movie").on("releases");
                table.integer("id").change();
                table.dropPrimary("id");
                table.primary({"id_layouting","id_localisation", "id_type", "id_movie"});
            });
            Schema::create("sync", [](Blueprint &table)
            {
                table.unsignedBigInteger("id_localisation");
                table.unsignedBigInteger("id_type");
                table.unsignedBigInteger("id_movie");
                table.unsignedBigInteger("id_layouting");

                table.foreign("id_layouting").references("id_layouting").on("layouting_configuration");
                table.foreign("id_localisation").references("id_localisation").on("releases");
                table.foreign("id_type").references("id_type").on("releases");
                table.foreign("id_movie").references("id_movie").on("releases");
                table.primary({"id_layouting","id_localisation", "id_type", "id_movie"});
            });
            DB::statement("CREATE TRIGGER `cpl_exists` \
            BEFORE INSERT ON `sync` \
            FOR EACH ROW \
            BEGIN \
                IF NOT EXISTS (SELECT * FROM `cpl` WHERE id_layouting = NEW.id_layouting && id_type = NEW.id_type && id_movie = NEW.id_movie && id_localisation = NEW.id_localisation) THEN \
                    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='Aucun CPL correspondant'; \
                END IF; \
            END;");
        }

        /*! Reverse the migrations. */
        void down() const override
        {
            DB::statement("DROP TRIGGER `cpl_exists`");
            Schema::dropIfExists("sync");
            Schema::table("cpl", [](Blueprint &table)
            {
                table.dropForeign("cpl_id_layouting_foreign");
                table.dropForeign("cpl_id_localisation_foreign");
                table.dropForeign("cpl_id_type_foreign");
                table.dropForeign("cpl_id_movie_foreign");
                table.dropPrimary({"id_layouting","id_localisation", "id_type", "id_movie"});
                table.dropColumns({"id_localisation", "id_type", "id_movie", "id_layouting"});
            });
            DB::statement("ALTER TABLE cpl AUTO_INCREMENT =4, MODIFY id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY");
            
            Schema::dropIfExists("releases");
            Schema::dropIfExists("localisation");
            Schema::dropIfExists("type");
            Schema::dropIfExists("movie");

            Schema::dropIfExists("projection_sever_configuration"); 
            Schema::dropIfExists("hallway_server_configuration");
            Schema::dropIfExists("auditorium_server_configuration");
            Schema::dropIfExists("default_server_configuration_hallway");
            Schema::dropIfExists("default_server_configuration_auditorium");
            Schema::dropIfExists("par");
            Schema::dropIfExists("par_menage");
            Schema::dropIfExists("back_light");
            Schema::dropIfExists("mouving_head");
            Schema::dropIfExists("panel");
            Schema::dropIfExists("layouting_configuration");
            Schema::dropIfExists("site");
            Schema::dropIfExists("location");
            Schema::dropIfExists("connection");
            Schema::dropIfExists("groups");

        }
    };

} // namespace Migrations