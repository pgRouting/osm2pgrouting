CREATE OR REPLACE FUNCTION pgr_brokenGraph(
                IN tbl character varying,
                IN clmn character varying
        )
        RETURNS SETOF VOID AS
$body$
DECLARE
        sql1     text;
        rec1    record;
        sql2     text;
        rec2    record;
        rec_count record;
        rec_single record;
        graph_id  integer;
        target  integer;
        gids int [];
        --point integer;
        
BEGIN   
        EXECUTE 'SELECT count(*) as count FROM information_schema.columns WHERE table_name= ''' || quote_ident(tbl) || ''' and column_name= ''' || quote_ident(clmn) || '''' INTO rec_count;
        If (rec_count.count > 0) THEN
                RAISE NOTICE 'Column % in the table % already exists!', quote_ident(clmn), quote_ident(tbl);
        ELSE
                EXECUTE 'ALTER TABLE ' || quote_ident(tbl) || ' ADD COLUMN ' || quote_ident(clmn) || ' INTEGER DEFAULT -1';
                EXECUTE 'ALTER TABLE ' || quote_ident(tbl) || ' ADD COLUMN garbage INTEGER DEFAULT 0';
                graph_id := 1;
                While true
                        Loop
                                -------- Assisgn the very first -1 row graph_id
                                EXECUTE 'select * from ' || quote_ident(tbl) || ' where ' || quote_ident(clmn) || ' = -1 limit 1' INTO rec_single;
                                EXECUTE 'update ' || quote_ident(tbl) || ' set ' || quote_ident(clmn) || ' = ' || graph_id || ' where gid = ' || rec_single.gid || '';

                                --------- search other rows with that particular graph_id
                                while true
                                        loop
                                                execute 'select count(*) from ' || quote_ident(tbl) || ' where ' || quote_ident(clmn) || ' = ' || graph_id || ' AND garbage = 0' into rec_count;
                                                if (rec_count.count > 0) then
                                                        sql1 := 'select * from ' || quote_ident(tbl) || ' where ' || quote_ident(clmn) || ' = ' || graph_id || ' AND garbage = 0';
                                                        for rec1 in execute sql1
                                                                loop
                                                                        sql2 := 'select * from ' || quote_ident(tbl) || ' where source = '|| rec1.source ||' or target = '|| rec1.source ||' or source = '|| rec1.target ||' or target = '|| rec1.target ||'';
                                                                        for rec2 in execute sql2
                                                                                loop
                                                                                        EXECUTE 'update ' || quote_ident(tbl) || ' set ' || quote_ident(clmn) || ' = ' || graph_id || ' where gid = ' || rec2.gid || '';
                                                                                end loop;
                                                                        execute 'update ' || quote_ident(tbl) || ' set garbage = 1 where gid = ' || rec1.gid || '';
                                                                end loop;
                                                else
                                                        exit;
                                                end if;
                                        end loop;
                                
                                ------ Following is to exit the while loop. 0 means no more -1 id.
                                EXECUTE 'select count(*) as count from ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = -1' INTO rec_count;
                                If (rec_count.count = 0) THEN
                                        EXIT;
                                ELSE
                                        graph_id := graph_id + 1;
                                END IF;
                        end loop;
        END IF;
        EXCEPTION
                WHEN internal_error THEN
                        RAISE NOTICE 'Something went wrong!';
END;
$body$
LANGUAGE 'plpgsql' VOLATILE STRICT;
