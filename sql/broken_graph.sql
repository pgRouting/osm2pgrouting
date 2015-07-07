CREATE OR REPLACE FUNCTION pgr_brokenGraph(
                IN tbl character varying,
                IN clmn character varying
        )
        RETURNS SETOF VOID AS
$body$
DECLARE
        sql     text;
        rec     record;
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
                ------------
                --sql := 'select count(*) from ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = -1';
                graph_id := 1;
                --FOR rec IN EXECUTE sql
                WHILE True
                        LOOP
                                -----------
                                EXECUTE 'select * from ' || quote_ident(tbl) || ' where ' || quote_ident(clmn) || ' = -1 limit 1' INTO rec_single;
                                gids  := array_append(gids, rec_single.gid);
                                WHILE True
                                        Loop
                                                source := rec_single.source;
                                                target := rec_single.target;
                                                sql := 'select * from ' || quote_ident(tbl) || ' where source = '|| source ||' or target = '|| source ||' or source = '|| target ||' or target = '|| target ||'';
                                                For rec in execute sql:
                                                        Loop
                                                                gids  := array_append(gids, rec.gid);
                                                        End Loop;
                                        End Loop;






                                
                                --EXECUTE 'update ' || quote_ident(tbl) || ' set ' || quote_ident(clmn) || ' = ' || graph_id || ' where gid = ' || rec_single.gid || '';
                                gids  := array_append(gids, rec_single.gid);
                                raise notice '%', graph_id;
                                raise notice '%', gids;
                                --graph_id := graph_id + 1;
                                -----------
                                EXECUTE 'select count(*) as count from ' || quote_ident(tbl) || ' WHERE ' || quote_ident(clmn) || ' = -1' INTO rec_count;
                                If (rec_count.count = 0) THEN
                                        RETURN;
                                ELSE
                                        graph_id := graph_id + 1;
                                END IF;
                        END LOOP;
                ------------
        END IF;
        EXCEPTION
                WHEN internal_error THEN
                        RAISE NOTICE 'Something went wrong!';
END;
$body$
LANGUAGE 'plpgsql' VOLATILE STRICT;